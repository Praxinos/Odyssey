// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SArianeEditorToolSelector.h"
#include "ArianeEditor.h"
#include "OdysseyStyle.h"
// Unreal
#include "Widgets/SBoxPanel.h"


/* Gary
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Input/SCheckBox.h"
*/

SArianeEditorToolSelector::~SArianeEditorToolSelector()
{
}

SArianeEditorToolSelector::SArianeEditorToolSelector()
{
}

void
SArianeEditorToolSelector::Construct(const FArguments& InArgs, FArianeEditor* iEditor)
{
    mOnToolSelected = InArgs._OnToolSelected;
    mEditor = iEditor;

    const FCheckBoxStyle* checkboxStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.ToggleButton");
    TSharedRef<SVerticalBox> verticalBox = SNew(SVerticalBox);

    for ( UArianeEditorTool* tool : mEditor->GetTools() )
    {
        verticalBox->AddSlot()
        .AutoHeight()
        .Padding(4)
        [
            SNew(SCheckBox)
            .Style( checkboxStyle )
            .IsFocusable(false)
            .OnCheckStateChanged(this, &SArianeEditorToolSelector::OnToolCheckStateChanged, tool)
            .IsChecked(this, &SArianeEditorToolSelector::IsToolChecked, tool)
            .Visibility(this, &SArianeEditorToolSelector::ToolVisibility, tool)
            .ToolTipText(this, &SArianeEditorToolSelector::ToolTooltip, tool)
            .Padding(FMargin(4.f))
            [
                SNew(SImage)
                .Image(&tool->Icon)
                .DesiredSizeOverride(FVector2D(20.f, 20.f))
            ]
        ];
    }

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    .Padding(4)
    [
        verticalBox
    ];
}

void
SArianeEditorToolSelector::OnToolCheckStateChanged(ECheckBoxState InValue, UArianeEditorTool* iTool)
{
    if (InValue == ECheckBoxState::Checked)
        mOnToolSelected.ExecuteIfBound(iTool);
}

EVisibility
SArianeEditorToolSelector::ToolVisibility(UArianeEditorTool* iTool) const
{
    return iTool->IsActivable() ? EVisibility::Visible : EVisibility::Collapsed;
}

ECheckBoxState
SArianeEditorToolSelector::IsToolChecked(UArianeEditorTool* iTool) const
{
    return iTool->IsActivated() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText
SArianeEditorToolSelector::ToolTooltip(UArianeEditorTool* iTool) const
{
    return iTool->GetTooltip();
}
