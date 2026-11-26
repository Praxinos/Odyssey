// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImageMigrationWorkflow.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "Editor.h"
#include "Misc/MessageDialog.h"

#include "OdysseyLegacyAssetGroup.h"
#include "OdysseyLegacyPackageConversion.h"
#include "OdysseyLegacyPackageScanner.h"
#include "OdysseyTiledImageMigrationSettings.h"
#include "SOdysseyTiledImageMigrationWindow.h"

#define LOCTEXT_NAMESPACE "OdysseyTiledImageMigration"

namespace UE::OdysseyTiledImageMigration::Private
{
/** A line of the conversion summary. */
struct FSummaryLine
{
    /** The outcome the line counts. */
    EOdysseyLegacyConversionOutcome Outcome;

    /** The text of the line, where {0} is the number of packages with the outcome. */
    FText Format;
};

/** Records that the user no longer wants the conversion offered at startup. */
void SaveDoNotAskAgain()
{
    UOdysseyTiledImageMigrationSettings* Settings = GetMutableDefault<UOdysseyTiledImageMigrationSettings>();
    Settings->bDoNotAskAgain = true;
    Settings->SaveConfig();
}

/** Returns the packages of the chosen groups. */
TArray<FOdysseyLegacyPackage> CollectChosenPackages(TConstArrayView<FOdysseyLegacyAssetGroup> Groups, TConstArrayView<int32> GroupIndexes)
{
    TArray<FOdysseyLegacyPackage> Packages;
    for (const int32 GroupIndex : GroupIndexes)
    {
        const FOdysseyLegacyAssetGroup& Group = Groups[GroupIndex];
        Packages.Append(Group.Packages);
    }
    return Packages;
}

/** Returns the per-outcome package counts of the conversion, as shown to the user. */
[[nodiscard]] FText CreateSummary(const FOdysseyLegacyConversionResult& Result)
{
    const FSummaryLine SummaryLines[] = {
        { EOdysseyLegacyConversionOutcome::Saved, LOCTEXT("summary.saved", "{0} assets converted and saved.") },
        { EOdysseyLegacyConversionOutcome::OpenInEditor, LOCTEXT("summary.open", "{0} assets skipped because they are open in an editor: save them from there, or close them and convert again.") },
        { EOdysseyLegacyConversionOutcome::HasUnsavedChanges, LOCTEXT("summary.unsaved", "{0} assets skipped because they have unsaved changes: save them, then convert again if needed.") },
        { EOdysseyLegacyConversionOutcome::CheckoutFailed, LOCTEXT("summary.checkout", "{0} assets not converted because their files cannot be written.") },
        { EOdysseyLegacyConversionOutcome::LoadFailed, LOCTEXT("summary.load", "{0} assets not converted because they cannot be loaded.") },
        { EOdysseyLegacyConversionOutcome::SaveFailed, LOCTEXT("summary.save", "{0} assets not converted because they cannot be saved.") },
        { EOdysseyLegacyConversionOutcome::StillLegacy, LOCTEXT("summary.still", "{0} assets saved but still in the older format.") },
        { EOdysseyLegacyConversionOutcome::NotProcessed, LOCTEXT("summary.not-processed", "{0} assets not processed because the conversion was canceled.") },
    };

    TArray<FText> Lines;
    Lines.Reserve(UE_ARRAY_COUNT(SummaryLines));
    for (const FSummaryLine& SummaryLine : SummaryLines)
    {
        const int32 Count = Result.Count(SummaryLine.Outcome);
        if (Count > 0)
        {
            const FText CountText = FText::AsNumber(Count);
            FText Line = FText::Format(SummaryLine.Format, CountText);
            Lines.Add(MoveTemp(Line));
        }
    }

    const FText Separator = FText::FromStringView(TEXTVIEW("\n"));
    return FText::Join(Separator, Lines);
}

/** Returns why the conversion cannot run now, or an empty text when it can. */
[[nodiscard]] FText CreateRefusal()
{
    const IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    if (Registry.IsLoadingAssets())
    {
        return LOCTEXT("workflow.scanning", "The editor is still discovering the assets of the project. Try again once it is done.");
    }

    if (GEditor->PlayWorld)
    {
        return LOCTEXT("workflow.pie", "The legacy assets cannot be converted while the game runs in the editor.");
    }
    return FText::GetEmpty();
}
}

namespace Odyssey::TiledImageMigration
{
void Run()
{
    using namespace UE::OdysseyTiledImageMigration::Private;
    check(IsInGameThread());
    const FText Title = LOCTEXT("workflow.title", "Convert Legacy Odyssey Assets");
    const FText Refusal = CreateRefusal();
    if (!Refusal.IsEmpty())
    {
        FMessageDialog::Open(EAppMsgType::Ok, Refusal, Title);
        return;
    }

    const TArray<const UClass*> OwnerClasses = GetRasterOwnerClasses();
    const TArray<FOdysseyLegacyPackage> Packages = FindLegacyPackages(OwnerClasses);
    const TArray<FOdysseyLegacyAssetGroup> Groups = CreateLegacyAssetGroups(Packages);
    if (Groups.IsEmpty())
    {
        const FText Message = LOCTEXT("workflow.nothing", "All the Odyssey assets of the project and its plugins use the current format.");
        FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
        return;
    }

    const FOdysseyTiledImageMigrationChoice Choice = SOdysseyTiledImageMigrationWindow::ShowModal(Groups);
    if (Choice.Decision == EOdysseyTiledImageMigrationDecision::DoNotAskAgain)
    {
        SaveDoNotAskAgain();
        return;
    }

    if (Choice.Decision != EOdysseyTiledImageMigrationDecision::Convert)
    {
        return;
    }

    const TArray<FOdysseyLegacyPackage> ChosenPackages = CollectChosenPackages(Groups, Choice.GroupIndexes);
    const FOdysseyLegacyConversionResult Result = ConvertLegacyPackages(ChosenPackages);
    const FText Summary = CreateSummary(Result);
    FMessageDialog::Open(EAppMsgType::Ok, Summary, Title);
}
}

#undef LOCTEXT_NAMESPACE
