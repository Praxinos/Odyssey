// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "TimelineTools/OdysseyAnimationTimelineCutTool.h"

#include "ScopedTransaction.h"

#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCutTool::~FOdysseyAnimationTimelineCutTool()
{
}

FOdysseyAnimationTimelineCutTool::FOdysseyAnimationTimelineCutTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition)
    : FOdysseyAnimationTimelineTool(iTimelinePosition)
{
}

FReply
FOdysseyAnimationTimelineCutTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
    if (iParams.mMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    if (!iParams.mLayer->IsEditable())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    return FReply::Handled();
}

FReply
FOdysseyAnimationTimelineCutTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    if (!iParams.mLayer->IsEditable())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
    int frame = (int)(MousePositionToFrame(posX) + 0.5f);

    UOdysseyLayerCell* cell = iParams.mLayer->GetCellAtFrame(frame);
    if (!cell || cell->GetFrameRange().GetLowerBoundValue() == frame)
        return FReply::Unhandled();

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.cut-tool.transaction.break-cell", "Break Cell"));
#endif
    UOdysseyLayerCell* newCell = cell->Break(frame - cell->GetFrameRange().GetLowerBoundValue(), !iParams.mMouseEvent.IsControlDown());
    if (!newCell)
        return FReply::Unhandled();

    //Remove mark from the new cell, because we consider the new cell will be modified by the user and will not represent the original cell anymore
    //This is an arbitrary choice, you are free to change this behaviour whenever you want without any side effect
    newCell->SetMarks( {} );

    return FReply::Handled();
}

FMouseCursor
FOdysseyAnimationTimelineCutTool::GetMouseCursor() const //override
{
    return EMouseCursorCustom::Scissor;
}

#undef LOCTEXT_NAMESPACE
