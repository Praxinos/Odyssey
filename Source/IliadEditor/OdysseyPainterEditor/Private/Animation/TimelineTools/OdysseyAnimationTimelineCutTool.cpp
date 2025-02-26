// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TimelineTools/OdysseyAnimationTimelineCutTool.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimationEditorTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCutTool::~FOdysseyAnimationTimelineCutTool()
{
}

FOdysseyAnimationTimelineCutTool::FOdysseyAnimationTimelineCutTool(TSharedRef<FOdysseyAnimationEditorTimelinePosition> iTimelinePosition)
    : FOdysseyAnimationTimelineTool(iTimelinePosition)
{
}

FReply
FOdysseyAnimationTimelineCutTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    if (iParams.mLayer->IsLockedRecursively())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
    int frame = (int)(MousePositionToFrame(posX) + 0.5f);

    UOdysseyAnimationCell* cell = iParams.mLayer->GetCellAtFrame(frame);
    if (!cell || cell->GetFrameRange().GetLowerBoundValue() == frame)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.cut-tool.transaction.break-cell", "Break Cell"));
#endif
    UOdysseyAnimationCell* newCell = cell->Break(frame - cell->GetFrameRange().GetLowerBoundValue(), !iParams.mMouseEvent.IsControlDown());
    if (!newCell)
        return FReply::Unhandled();

    //Remove mark from the new cell, because we consider the new cell will be modified by the user and will not represent the original cell anymore
    //This is an arbitrary choice, you are free to change this behaviour whenever you want without any side effect
    FOdysseyObjectEditorUtils::SetPropertyValue(newCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), INDEX_NONE);

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
