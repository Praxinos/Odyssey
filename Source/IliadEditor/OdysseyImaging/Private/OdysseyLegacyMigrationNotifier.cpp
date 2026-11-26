// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyLegacyMigrationNotifier.h"

#include "Containers/Ticker.h"
#include "CoreGlobals.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/App.h"
#include "UObject/Package.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "Imaging"

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyLegacyMigration, Log, All);

namespace UE::OdysseyImaging::Private
{
/** The reported packages not notified yet. */
TArray<TWeakObjectPtr<UPackage>> GPendingPackages;

/** The ticker notifying the pending packages; registered while some are pending. */
FTSTicker::FDelegateHandle GTickerHandle;
}

//static
void
FOdysseyLegacyMigrationNotifier::Add(TNotNull<UPackage*> Package)
{
    using namespace UE::OdysseyImaging::Private;
    check(IsInGameThread());
    if (!GIsEditor || IsRunningCommandlet() || FApp::IsUnattended() || GIsAutomationTesting)
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
    const FTickerDelegate Delegate = FTickerDelegate::CreateStatic(&FOdysseyLegacyMigrationNotifier::Tick);
    GTickerHandle = Ticker.AddTicker(Delegate);
}

//static
void
FOdysseyLegacyMigrationNotifier::Shutdown()
{
    using namespace UE::OdysseyImaging::Private;
    if (GTickerHandle.IsValid())
    {
        FTSTicker::RemoveTicker(GTickerHandle);
        GTickerHandle.Reset();
    }
    GPendingPackages.Empty();
}

//static
bool
FOdysseyLegacyMigrationNotifier::Tick(float DeltaTime)
{
    using namespace UE::OdysseyImaging::Private;
    // The editor forbids dirtying a package while packages load.
    if (UE::GetIsEditorLoadingPackage() || IsAsyncLoading())
    {
        return true;
    }

    const TArray<TWeakObjectPtr<UPackage>> Packages = MoveTemp(GPendingPackages);
    GPendingPackages.Reset();
    GTickerHandle.Reset();
    for (const TWeakObjectPtr<UPackage>& WeakPackage : Packages)
    {
        if (UPackage* Package = WeakPackage.Get())
        {
            Notify(Package);
        }
        else
        {
            UE_LOGF(LogOdysseyLegacyMigration, Verbose, "A converted package was unloaded before it could be notified.");
        }
    }
    // Removes the ticker.
    return false;
}

//static
void
FOdysseyLegacyMigrationNotifier::Notify(TNotNull<UPackage*> Package)
{
    Package->SetDirtyFlag(true);
    const FString PackageName = Package->GetName();
    UE_LOGF(LogOdysseyLegacyMigration, Display, "%ls was converted from a legacy Odyssey storage: save it to keep the conversion.", *PackageName);

    const FText PackageText = FText::FromString(PackageName);
    const FText Format = LOCTEXT("legacy-migration.notification", "{0} was converted from an older Odyssey version. Save it to keep the conversion and load it faster.");
    const FText Message = FText::Format(Format, PackageText);
    FNotificationInfo Info(Message);
    Info.ExpireDuration = 8.0f;
    FSlateNotificationManager& NotificationManager = FSlateNotificationManager::Get();
    NotificationManager.AddNotification(Info);
}

#undef LOCTEXT_NAMESPACE
