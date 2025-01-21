// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Input/SCheckBox.h"

SOdysseyPainterEditorToolsTileView::~SOdysseyPainterEditorToolsTileView()
{
}

SOdysseyPainterEditorToolsTileView::SOdysseyPainterEditorToolsTileView()
{
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyPainterEditorToolsTileView::Construct(const FArguments& InArgs)
{
    mTools = InArgs._Tools;
    mOnToolSelected = InArgs._OnToolSelected;

    const FCheckBoxStyle* checkboxStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.ToggleButton");
    TSharedRef<SVerticalBox> verticalBox = SNew(SVerticalBox);
    for (UOdysseyPainterEditorTool* tool : mTools)
    {
        verticalBox->AddSlot()
        .AutoHeight()
        [
            SNew(SCheckBox)
            .Style( checkboxStyle )
            .OnCheckStateChanged(this, &SOdysseyPainterEditorToolsTileView::OnToolCheckStateChanged, tool)
            .IsChecked(this, &SOdysseyPainterEditorToolsTileView::IsToolChecked, tool)
            .Visibility(this, &SOdysseyPainterEditorToolsTileView::ToolVisibility, tool)
            .ToolTipText(this, &SOdysseyPainterEditorToolsTileView::ToolTooltip, tool)
            .Padding(FMargin(2.f))
            [
                SNew(SImage)
                .Image(&tool->Icon)
                .DesiredSizeOverride(FVector2D(32.f, 32.f))
            ]
        ];
    }

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    [
        verticalBox
    ];
}

void
SOdysseyPainterEditorToolsTileView::OnToolCheckStateChanged(ECheckBoxState InValue, UOdysseyPainterEditorTool* iTool)
{
    if (InValue == ECheckBoxState::Checked)
        mOnToolSelected.ExecuteIfBound(iTool);
}

EVisibility
SOdysseyPainterEditorToolsTileView::ToolVisibility(UOdysseyPainterEditorTool* iTool) const
{
    return !iTool->mIsTemporaryTool && iTool->IsActivable() ? EVisibility::Visible : EVisibility::Collapsed;
}

ECheckBoxState
SOdysseyPainterEditorToolsTileView::IsToolChecked(UOdysseyPainterEditorTool* iTool) const
{
    return iTool->IsActivated() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText
SOdysseyPainterEditorToolsTileView::ToolTooltip(UOdysseyPainterEditorTool* iTool) const
{
    return iTool->GetTooltip();
}
