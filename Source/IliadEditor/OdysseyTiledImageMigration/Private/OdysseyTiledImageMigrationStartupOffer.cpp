// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImageMigrationStartupOffer.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "OdysseyLegacyAssetGroup.h"
#include "OdysseyTiledImageMigrationSettings.h"
#include "OdysseyTiledImageMigrationWorkflow.h"
#include "OdysseyLegacyPackageScanner.h"
#include "OdysseyUserInteraction.h"
#include "OdysseyTiledImageMigrationLog.h"

#define LOCTEXT_NAMESPACE "OdysseyTiledImageMigration"

namespace UE::OdysseyTiledImageMigration::Private
{
/** How long the notification fades out once the user clicked one of its buttons, in seconds. */
constexpr float CloseFadeOutDuration = 0.1f;

/** Returns the number of legacy packages in the project content. */
int32 CountProjectPackages()
{
    const TArray<FTopLevelAssetPath> OwnerClasses = Odyssey::TiledImageMigration::GetRasterOwnerClasses();
    const TArray<FOdysseyLegacyPackage> Packages = Odyssey::TiledImageMigration::FindLegacyPackages(OwnerClasses);
    const TArray<FOdysseyLegacyAssetGroup> Groups = Odyssey::TiledImageMigration::CreateLegacyAssetGroups(Packages);
    int32 Count = 0;
    for (const FOdysseyLegacyAssetGroup& Group : Groups)
    {
        if (Group.IsProject())
        {
            Count += Group.Packages.Num();
        }
    }
    return Count;
}
}

void
FOdysseyTiledImageMigrationStartupOffer::Start()
{
    const UOdysseyTiledImageMigrationSettings* Settings = GetDefault<UOdysseyTiledImageMigrationSettings>();
    if (!Odyssey::CanInteractWithUser() || Settings->bDoNotAskAgain)
    {
        return;
    }

    FAssetRegistryModule& RegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& Registry = RegistryModule.Get();
    // OnFilesLoaded is broadcast once, at the end of the initial scan, and never to a later subscriber.
    if (Registry.IsLoadingAssets())
    {
        IAssetRegistry::FFilesLoadedEvent& FilesLoadedEvent = Registry.OnFilesLoaded();
        FilesLoadedHandle = FilesLoadedEvent.AddRaw(this, &FOdysseyTiledImageMigrationStartupOffer::OnFilesLoaded);
        return;
    }
    OnFilesLoaded();
}

void
FOdysseyTiledImageMigrationStartupOffer::Stop()
{
    // The asset registry module may already be unloaded at shutdown.
    if (FilesLoadedHandle.IsValid())
    {
        if (FAssetRegistryModule* RegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>("AssetRegistry"))
        {
            IAssetRegistry& Registry = RegistryModule->Get();
            IAssetRegistry::FFilesLoadedEvent& FilesLoadedEvent = Registry.OnFilesLoaded();
            FilesLoadedEvent.Remove(FilesLoadedHandle);
        }
        FilesLoadedHandle.Reset();
    }

    if (OfferTickerHandle.IsValid())
    {
        FTSTicker::RemoveTicker(OfferTickerHandle);
        OfferTickerHandle.Reset();
    }
    CloseNotification();
}

void
FOdysseyTiledImageMigrationStartupOffer::OnFilesLoaded()
{
    // The offer scans the registry, which is not done from within a callback of the registry.
    FTSTicker& Ticker = FTSTicker::GetCoreTicker();
    const FTickerDelegate Delegate = FTickerDelegate::CreateRaw(this, &FOdysseyTiledImageMigrationStartupOffer::Offer);
    OfferTickerHandle = Ticker.AddTicker(Delegate);
}

bool
FOdysseyTiledImageMigrationStartupOffer::Offer(float DeltaTime)
{
    // Removed by returning false.
    OfferTickerHandle.Reset();
    const int32 PackageCount = UE::OdysseyTiledImageMigration::Private::CountProjectPackages();
    if (PackageCount > 0)
    {
        ShowNotification(PackageCount);
    }
    return false;
}

void
FOdysseyTiledImageMigrationStartupOffer::ShowNotification(int32 PackageCount)
{
    const FText CountText = FText::AsNumber(PackageCount);
    const FText Format = LOCTEXT("offer.message", "{0} Odyssey assets use an older format. Converting them once makes them open faster.");
    const FText Message = FText::Format(Format, CountText);
    FNotificationInfo Info(Message);
    Info.bFireAndForget = false;
    Info.bUseSuccessFailIcons = false;
    Info.WidthOverride = 420.0f;
    const FSimpleDelegate OnReviewDelegate = FSimpleDelegate::CreateRaw(this, &FOdysseyTiledImageMigrationStartupOffer::OnReview);
    const FSimpleDelegate OnLaterDelegate = FSimpleDelegate::CreateRaw(this, &FOdysseyTiledImageMigrationStartupOffer::CloseNotification);
    const FText ReviewText = LOCTEXT("offer.review", "Review...");
    const FText ReviewToolTip = LOCTEXT("offer.review-tooltip", "Shows the legacy assets of the project and its plugins, and converts those you choose.");
    Info.ButtonDetails.Emplace(ReviewText, ReviewToolTip, OnReviewDelegate, SNotificationItem::CS_Pending);
    const FText LaterText = LOCTEXT("offer.later", "Later");
    const FText LaterToolTip = LOCTEXT("offer.later-tooltip", "Closes this notification; the conversion is offered again at the next startup.");
    Info.ButtonDetails.Emplace(LaterText, LaterToolTip, OnLaterDelegate, SNotificationItem::CS_Pending);

    FSlateNotificationManager& NotificationManager = FSlateNotificationManager::Get();
    const TSharedPtr<SNotificationItem> Item = NotificationManager.AddNotification(Info);
    if (!Item)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "The notification offering the conversion of %d legacy Odyssey assets cannot be shown.", PackageCount);
        return;
    }

    Item->SetCompletionState(SNotificationItem::CS_Pending);
    Notification = Item;
}

void
FOdysseyTiledImageMigrationStartupOffer::OnReview()
{
    CloseNotification();
    Odyssey::TiledImageMigration::Run();
}

void
FOdysseyTiledImageMigrationStartupOffer::CloseNotification()
{
    if (const TSharedPtr<SNotificationItem> Item = Notification.Pin())
    {
        Item->SetFadeOutDuration(UE::OdysseyTiledImageMigration::Private::CloseFadeOutDuration);
        Item->Fadeout();
    }
    Notification.Reset();
}

#undef LOCTEXT_NAMESPACE
