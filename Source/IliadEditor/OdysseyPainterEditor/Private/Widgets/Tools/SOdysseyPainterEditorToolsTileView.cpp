// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
        .Padding(4)
        [
            SNew(SCheckBox)
            .Style( checkboxStyle )
            .OnCheckStateChanged(this, &SOdysseyPainterEditorToolsTileView::OnToolCheckStateChanged, tool)
            .IsChecked(this, &SOdysseyPainterEditorToolsTileView::IsToolChecked, tool)
            .Visibility(this, &SOdysseyPainterEditorToolsTileView::ToolVisibility, tool)
            .ToolTipText(this, &SOdysseyPainterEditorToolsTileView::ToolTooltip, tool)
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
