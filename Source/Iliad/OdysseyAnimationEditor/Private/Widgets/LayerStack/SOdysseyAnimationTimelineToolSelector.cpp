// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineToolSelector.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineToolSelector::Construct(const FArguments& iArgs)
{
    ChildSlot
    [
        SNew(SSegmentedControl<EOdysseyTimelineTool>)
        .Value(this, &SOdysseyAnimationTimelineToolSelector::GetCurrentTool)
        .OnValueChecked(this, &SOdysseyAnimationTimelineToolSelector::OnToolChecked)

        //Selection Tool
        + SSegmentedControl<EOdysseyTimelineTool>::Slot(EOdysseyTimelineTool::Selection)
        .Icon(FOdysseyStyle::GetBrush( "Animation.Timeline.Tools.Selection" ))
        .ToolTip(LOCTEXT("timeline.selection-tool.tooltip", "Selection Tool"))

        //Move Tool
        + SSegmentedControl<EOdysseyTimelineTool>::Slot(EOdysseyTimelineTool::Move)
        .Icon(FOdysseyStyle::GetBrush( "Animation.Timeline.Tools.Move" ))
        .ToolTip(LOCTEXT("timeline.move-tool.tooltip", "Move Tool"))

        //Cut Tool
        + SSegmentedControl<EOdysseyTimelineTool>::Slot(EOdysseyTimelineTool::Cut)
        .Icon(FOdysseyStyle::GetBrush( "Animation.Timeline.Tools.Cut" ))
        .ToolTip(LOCTEXT("timeline.cut-tool.tooltip", "Cut Tool"))
    ];
}

EOdysseyTimelineTool
SOdysseyAnimationTimelineToolSelector::GetCurrentTool() const
{
    return FOdysseyAnimationTimelineTools::Get().GetCurrentTool();
}

void
SOdysseyAnimationTimelineToolSelector::OnToolChecked(EOdysseyTimelineTool iTool, ECheckBoxState iState)
{
    if (iState == ECheckBoxState::Checked)
        FOdysseyAnimationTimelineTools::Get().SetCurrentTool(iTool);
}

#undef LOCTEXT_NAMESPACE
