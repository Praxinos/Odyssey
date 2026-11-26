// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyLegacyMigrationNotifier.h"

#include "Containers/Ticker.h"
#include "CoreGlobals.h"
#include "FileHelpers.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/PlatformTime.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/Package.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "OdysseyUserInteraction.h"

#define LOCTEXT_NAMESPACE "Imaging"

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyLegacyMigration, Log, All);

namespace UE::OdysseyImaging::Private
{
/**
 * How long no package must have been loading before the notification is shown, in seconds of real time:
 * the asset editor of a converted asset opens in the frames that follow its load, and the notification must show in it.
 */
constexpr double QuietDelay = 1.0;

/** How long the notification fades out once the user clicked one of its buttons, in seconds. */
constexpr float CloseFadeOutDuration = 0.1f;

/** The reported packages not notified yet. */
TArray<TWeakObjectPtr<UPackage>> GPendingPackages;

/** The ticker notifying the pending packages; registered while some are pending. */
FTSTicker::FDelegateHandle GTickerHandle;

/** The time from which no package has been loading, or a negative value while packages load. */
double GQuietSince = -1.0;

/** The subscription to package saves; registered while some packages are pending. */
FDelegateHandle GPackageSavedHandle;

/** Drops the report of a package saved to its file: it no longer needs to be saved. */
void OnPackageSaved(const FString& Filename, UPackage* Package, FObjectPostSaveContext SaveContext)
{
    // Cooks, procedural saves and autosaves do not write the package file the user opens.
    if (SaveContext.IsCooking() || SaveContext.IsProceduralSave() || SaveContext.IsFromAutoSave() || !SaveContext.IsUpdatingLoadedPath())
    {
        return;
    }
    GPendingPackages.Remove(Package);
}

/** Stops waiting for the end of loads: removes the ticker and the save subscription. */
void StopWaiting()
{
    if (GTickerHandle.IsValid())
    {
        FTSTicker::RemoveTicker(GTickerHandle);
        GTickerHandle.Reset();
    }

    if (GPackageSavedHandle.IsValid())
    {
        UPackage::PackageSavedWithContextEvent.Remove(GPackageSavedHandle);
        GPackageSavedHandle.Reset();
    }
    GQuietSince = -1.0;
}

/** Returns the reported packages that still exist. */
TArray<UPackage*> FindLivePackages(TConstArrayView<TWeakObjectPtr<UPackage>> WeakPackages)
{
    TArray<UPackage*> Packages;
    Packages.Reserve(WeakPackages.Num());
    for (const TWeakObjectPtr<UPackage>& WeakPackage : WeakPackages)
    {
        if (UPackage* Package = WeakPackage.Get())
        {
            Packages.Add(Package);
        }
        else
        {
            UE_LOGF(LogOdysseyLegacyMigration, Verbose, "A converted package was unloaded before it could be offered for saving.");
        }
    }
    return Packages;
}

/** Closes the notification at once, so that the click visibly takes effect. */
void CloseNotification(TWeakPtr<SNotificationItem> WeakItem)
{
    if (const TSharedPtr<SNotificationItem> Item = WeakItem.Pin())
    {
        // ExpireAndFadeout() would wait 1 s, then fade out for 2 s.
        Item->SetFadeOutDuration(CloseFadeOutDuration);
        Item->Fadeout();
    }
    else
    {
        UE_LOGF(LogOdysseyLegacyMigration, Warning, "The notification of the converted packages no longer exists.");
    }
}

/** Saves the reported packages that still exist, then closes the notification. */
void SavePackages(TConstArrayView<TWeakObjectPtr<UPackage>> WeakPackages, TWeakPtr<SNotificationItem> WeakItem)
{
    const TArray<UPackage*> Packages = FindLivePackages(WeakPackages);
    FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
    SaveParams.bCheckDirty = true;
    const FEditorFileUtils::EPromptReturnCode Result = FEditorFileUtils::PromptForCheckoutAndSave(Packages, SaveParams);
    if (Result != FEditorFileUtils::PR_Success)
    {
        UE_LOGF(LogOdysseyLegacyMigration, Warning, "%d converted packages were not all saved (result %d).", Packages.Num(), static_cast<int32>(Result));
    }
    CloseNotification(WeakItem);
}

/** Returns the message of the notification listing the packages. */
[[nodiscard]] FText CreateMessage(TConstArrayView<UPackage*> Packages)
{
    if (Packages.Num() == 1)
    {
        const FString PackageName = Packages[0]->GetName();
        const FText PackageNameText = FText::FromString(PackageName);
        const FText Format = LOCTEXT("legacy-migration.notification.one", "{0} was converted from an older Odyssey version. Save it to keep the conversion and load it faster.");
        return FText::Format(Format, PackageNameText);
    }

    const int32 PackageCount = Packages.Num();
    const FText PackageCountText = FText::AsNumber(PackageCount);
    const FText Format = LOCTEXT("legacy-migration.notification.many", "{0} packages were converted from an older Odyssey version. Save them to keep the conversion and load them faster.");
    return FText::Format(Format, PackageCountText);
}

/** Returns the names of the packages, one per line. */
[[nodiscard]] FText CreatePackageList(TConstArrayView<UPackage*> Packages)
{
    TArray<FText> Names;
    Names.Reserve(Packages.Num());
    for (const UPackage* Package : Packages)
    {
        const FString PackageName = Package->GetName();
        FText Name = FText::FromString(PackageName);
        Names.Add(MoveTemp(Name));
    }
    const FText Separator = FText::FromStringView(TEXTVIEW("\n"));
    return FText::Join(Separator, Names);
}

/** Marks the packages dirty and shows one notification, which offers to save them, in the active window. */
void NotifyPackages(TConstArrayView<UPackage*> Packages)
{
    TArray<TWeakObjectPtr<UPackage>> WeakPackages;
    WeakPackages.Reserve(Packages.Num());
    for (UPackage* Package : Packages)
    {
        Package->SetDirtyFlag(true);
        UE_LOGF(LogOdysseyLegacyMigration, Display, "%ls was converted from a legacy Odyssey storage: save it to keep the conversion.", *Package->GetName());
        WeakPackages.Add(Package);
    }

    // The buttons are created before the item they close.
    const TSharedRef<TWeakPtr<SNotificationItem>> ItemHolder = MakeShared<TWeakPtr<SNotificationItem>>();
    const FSimpleDelegate OnSave = FSimpleDelegate::CreateLambda([WeakPackages = MoveTemp(WeakPackages), ItemHolder]() -> void
    {
        SavePackages(WeakPackages, *ItemHolder);
    });
    const FSimpleDelegate OnDismiss = FSimpleDelegate::CreateLambda([ItemHolder]() -> void
    {
        CloseNotification(*ItemHolder);
    });

    const FText Message = CreateMessage(Packages);
    FNotificationInfo Info(Message);
    if (Packages.Num() > 1)
    {
        Info.SubText = CreatePackageList(Packages);
    }
    Info.bFireAndForget = false;
    Info.bUseSuccessFailIcons = false;
    Info.WidthOverride = 420.0f;
    FSlateApplication& Application = FSlateApplication::Get();
    Info.ForWindow = Application.GetActiveTopLevelRegularWindow();
    const FText SaveText = LOCTEXT("legacy-migration.notification.save", "Save");
    const FText SaveToolTip = LOCTEXT("legacy-migration.notification.save-tooltip", "Saves the converted packages.");
    Info.ButtonDetails.Emplace(SaveText, SaveToolTip, OnSave, SNotificationItem::CS_Pending);
    const FText DismissText = LOCTEXT("legacy-migration.notification.dismiss", "Dismiss");
    const FText DismissToolTip = LOCTEXT("legacy-migration.notification.dismiss-tooltip", "Closes this notification; the packages stay unsaved.");
    Info.ButtonDetails.Emplace(DismissText, DismissToolTip, OnDismiss, SNotificationItem::CS_Pending);

    FSlateNotificationManager& NotificationManager = FSlateNotificationManager::Get();
    const TSharedPtr<SNotificationItem> Item = NotificationManager.AddNotification(Info);
    if (!Item)
    {
        UE_LOGF(LogOdysseyLegacyMigration, Warning, "Cannot show the notification of the %d converted packages.", Packages.Num());
        return;
    }

    Item->SetCompletionState(SNotificationItem::CS_Pending);
    *ItemHolder = Item;
}

/** Notifies the pending packages once no package has been loading for QuietDelay. Returns true to keep ticking until then. */
bool Tick(float DeltaTime)
{
    // The editor forbids dirtying a package while packages load.
    if (UE::GetIsEditorLoadingPackage() || IsAsyncLoading())
    {
        GQuietSince = -1.0;
        return true;
    }

    // Real time rather than DeltaTime: the first tick after a long load has the duration of the load as DeltaTime.
    const double Now = FPlatformTime::Seconds();
    if (GQuietSince < 0.0)
    {
        GQuietSince = Now;
    }

    if (Now - GQuietSince < QuietDelay)
    {
        return true;
    }

    const TArray<TWeakObjectPtr<UPackage>> WeakPackages = MoveTemp(GPendingPackages);
    GPendingPackages.Reset();
    // The ticker is removed by returning false.
    GTickerHandle.Reset();
    StopWaiting();
    const TArray<UPackage*> Packages = FindLivePackages(WeakPackages);
    if (!Packages.IsEmpty())
    {
        NotifyPackages(Packages);
    }
    return false;
}
}

//static
void
FOdysseyLegacyMigrationNotifier::Add(TNotNull<UPackage*> Package)
{
    using namespace UE::OdysseyImaging::Private;
    check(IsInGameThread());
    if (!Odyssey::CanInteractWithUser())
    {
        return;
    }

    UPackage* const ReportedPackage = Package;
    GPendingPackages.AddUnique(ReportedPackage);
    if (GTickerHandle.IsValid())
    {
        return;
    }

    FTSTicker& Ticker = FTSTicker::GetCoreTicker();
    const FTickerDelegate Delegate = FTickerDelegate::CreateStatic(&Tick);
    GTickerHandle = Ticker.AddTicker(Delegate);
    GPackageSavedHandle = UPackage::PackageSavedWithContextEvent.AddStatic(&OnPackageSaved);
}

//static
void
FOdysseyLegacyMigrationNotifier::Shutdown()
{
    using namespace UE::OdysseyImaging::Private;
    StopWaiting();
    GPendingPackages.Empty();
}

#undef LOCTEXT_NAMESPACE
