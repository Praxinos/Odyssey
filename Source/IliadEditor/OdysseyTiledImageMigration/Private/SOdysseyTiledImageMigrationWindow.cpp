// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "SOdysseyTiledImageMigrationWindow.h"

#include "Editor.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"

#include "OdysseyTiledImageMigrationLog.h"

#define LOCTEXT_NAMESPACE "OdysseyTiledImageMigration"

namespace UE::OdysseyTiledImageMigration::Private
{
/** The padding around and between the parts of the window. */
constexpr float WindowPadding = 12.0f;

/** Returns why the group cannot be checked, or needs a checkout; empty otherwise. */
[[nodiscard]] FText CreateGroupStateText(EOdysseyLegacyAssetGroupState State)
{
    switch (State)
    {
        case EOdysseyLegacyAssetGroupState::EnginePlugin:
        {
            return LOCTEXT("window.engine-plugin", "Installed in the engine: the project cannot modify it.");
        }
        case EOdysseyLegacyAssetGroupState::NeedsCheckout:
        {
            return LOCTEXT("window.needs-checkout", "The assets are checked out from source control before the conversion.");
        }
        case EOdysseyLegacyAssetGroupState::Convertible:
        default:
        {
            return FText::GetEmpty();
        }
    }
}
}

void
SOdysseyTiledImageMigrationWindow::Construct(const FArguments& InArgs)
{
    Groups = InArgs._Groups;
    ParentWindow = InArgs._ParentWindow;
    CheckedGroups.Reserve(Groups.Num());
    for (const FOdysseyLegacyAssetGroup& Group : Groups)
    {
        const bool bChecked = Group.IsProject() && Group.State != EOdysseyLegacyAssetGroupState::EnginePlugin;
        CheckedGroups.Add(bChecked);
    }

    const TSharedRef<SVerticalBox> GroupRows = SNew(SVerticalBox);
    for (int32 GroupIndex = 0; GroupIndex < Groups.Num(); ++GroupIndex)
    {
        const TSharedRef<SWidget> Row = CreateGroupRow(GroupIndex);
        GroupRows->AddSlot()
            .AutoHeight()
            .Padding(0.0f, 4.0f)
            [
                Row
            ];
    }

    const float Padding = UE::OdysseyTiledImageMigration::Private::WindowPadding;
    const FSlateBrush* BorderBrush = FAppStyle::GetBrush("ToolPanel.GroupBorder");
    const TSharedRef<SWidget> Buttons = CreateButtons();
    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(BorderBrush)
        .Padding(Padding)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 0.0f, 0.0f, Padding)
            [
                SNew(STextBlock)
                .AutoWrapText(true)
                .Text(LOCTEXT("window.explanation", "These Odyssey assets were saved with an older version of Odyssey. Converting them now saves them in the current format, so that they open faster.\n\nThe conversion can take a long time, as each asset is loaded and saved, and is done once. Assets open in an editor or with unsaved changes are skipped."))
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                GroupRows
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Right)
            .Padding(0.0f, Padding, 0.0f, 0.0f)
            [
                Buttons
            ]
        ]
    ];
}

//static
FOdysseyTiledImageMigrationChoice
SOdysseyTiledImageMigrationWindow::ShowModal(TArray<FOdysseyLegacyAssetGroup> InGroups)
{
    const TSharedRef<SWindow> Window = SNew(SWindow)
        .Title(LOCTEXT("window.title", "Convert Legacy Odyssey Assets"))
        .SizingRule(ESizingRule::Autosized)
        .SupportsMaximize(false)
        .SupportsMinimize(false);
    const TSharedRef<SOdysseyTiledImageMigrationWindow> Widget = SNew(SOdysseyTiledImageMigrationWindow)
        .Groups(MoveTemp(InGroups))
        .ParentWindow(Window);
    Window->SetContent(Widget);
    GEditor->EditorAddModalWindow(Window);
    return Widget->Choice;
}

TSharedRef<SWidget>
SOdysseyTiledImageMigrationWindow::CreateGroupRow(int32 GroupIndex)
{
    const FOdysseyLegacyAssetGroup& Group = Groups[GroupIndex];
    const int32 PackageCount = Group.Packages.Num();
    const FText Count = FText::AsNumber(PackageCount);
    const FText Size = FText::AsMemory(Group.DiskSize);
    const FText Format = LOCTEXT("window.group", "{0}: {1} assets, {2}");
    const FText Label = FText::Format(Format, Group.DisplayName, Count, Size);
    const FText StateText = UE::OdysseyTiledImageMigration::Private::CreateGroupStateText(Group.State);
    const bool bCanCheck = Group.State != EOdysseyLegacyAssetGroupState::EnginePlugin;

    const TSharedRef<SVerticalBox> Row = SNew(SVerticalBox);
    Row->AddSlot()
        .AutoHeight()
        [
            SNew(SCheckBox)
            .IsEnabled(bCanCheck)
            .IsChecked_Lambda([this, GroupIndex]() -> ECheckBoxState
            {
                if (CheckedGroups[GroupIndex])
                {
                    return ECheckBoxState::Checked;
                }
                return ECheckBoxState::Unchecked;
            })
            .OnCheckStateChanged_Lambda([this, GroupIndex](ECheckBoxState State) -> void
            {
                CheckedGroups[GroupIndex] = State == ECheckBoxState::Checked;
            })
            [
                SNew(STextBlock)
                .Text(Label)
            ]
        ];

    if (!StateText.IsEmpty())
    {
        const FSlateColor SubduedColor = FSlateColor::UseSubduedForeground();
        Row->AddSlot()
            .AutoHeight()
            .Padding(24.0f, 0.0f, 0.0f, 0.0f)
            [
                SNew(STextBlock)
                .Text(StateText)
                .ColorAndOpacity(SubduedColor)
            ];
    }
    return Row;
}

TSharedRef<SWidget>
SOdysseyTiledImageMigrationWindow::CreateButtons()
{
    const FMargin SlotPadding = FAppStyle::GetMargin("StandardDialog.SlotPadding");
    return SNew(SUniformGridPanel)
        .SlotPadding(SlotPadding)
        + SUniformGridPanel::Slot(0, 0)
        [
            SNew(SButton)
            .HAlign(HAlign_Center)
            .IsEnabled_Lambda([this]() -> bool
            {
                return IsAnyGroupChecked();
            })
            .Text(LOCTEXT("window.convert", "Convert and Save"))
            .OnClicked(this, &SOdysseyTiledImageMigrationWindow::Close, EOdysseyTiledImageMigrationDecision::Convert)
        ]
        + SUniformGridPanel::Slot(1, 0)
        [
            SNew(SButton)
            .HAlign(HAlign_Center)
            .Text(LOCTEXT("window.later", "Later"))
            .OnClicked(this, &SOdysseyTiledImageMigrationWindow::Close, EOdysseyTiledImageMigrationDecision::Later)
        ]
        + SUniformGridPanel::Slot(2, 0)
        [
            SNew(SButton)
            .HAlign(HAlign_Center)
            .Text(LOCTEXT("window.do-not-ask", "Do Not Ask Again"))
            .ToolTipText(LOCTEXT("window.do-not-ask-tooltip", "The conversion stays available from the Tools menu, and can be offered again in Editor Preferences > Plugins > Odyssey - Legacy Assets."))
            .OnClicked(this, &SOdysseyTiledImageMigrationWindow::Close, EOdysseyTiledImageMigrationDecision::DoNotAskAgain)
        ];
}

FReply
SOdysseyTiledImageMigrationWindow::Close(EOdysseyTiledImageMigrationDecision Decision)
{
    Choice.Decision = Decision;
    Choice.GroupIndexes.Reset();
    Choice.GroupIndexes.Reserve(Groups.Num());
    if (Decision == EOdysseyTiledImageMigrationDecision::Convert)
    {
        for (int32 GroupIndex = 0; GroupIndex < Groups.Num(); ++GroupIndex)
        {
            if (CheckedGroups[GroupIndex])
            {
                Choice.GroupIndexes.Add(GroupIndex);
            }
        }
    }

    const TSharedPtr<SWindow> Window = ParentWindow.Pin();
    if (!Window)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "The conversion window is already closed; decision %d is recorded without closing it.", static_cast<int32>(Decision));
        return FReply::Handled();
    }

    Window->RequestDestroyWindow();
    return FReply::Handled();
}

bool
SOdysseyTiledImageMigrationWindow::IsAnyGroupChecked() const
{
    return CheckedGroups.Contains(true);
}

#undef LOCTEXT_NAMESPACE
