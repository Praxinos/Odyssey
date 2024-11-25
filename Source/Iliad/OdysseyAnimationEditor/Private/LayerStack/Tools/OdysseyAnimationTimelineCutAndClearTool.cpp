// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineCutAndClearTool.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimationEditorTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCutAndClearTool::~FOdysseyAnimationTimelineCutAndClearTool()
{
}

FOdysseyAnimationTimelineCutAndClearTool::FOdysseyAnimationTimelineCutAndClearTool(TSharedRef<FOdysseyAnimationEditorTimelinePosition> iTimelinePosition)
    : mTimelinePosition(iTimelinePosition)
{
}

FReply
FOdysseyAnimationTimelineCutAndClearTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    if (iParams.mLayer->IsLockedRecursively())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
    int frame = (int)(mTimelinePosition->MousePositionToFrame(posX) + 0.5f);

    UOdysseyAnimationCell* cell = iParams.mLayer->GetCellAtFrame(frame);
    if (!cell || cell->GetFrameRange().GetLowerBoundValue() == frame)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.cut-and-clear-tool.transaction.break-cell", "Break Cell And Clear"));
#endif
    UOdysseyAnimationCell* newCell = cell->Break(frame - cell->GetFrameRange().GetLowerBoundValue(), true);
    if (!newCell)
        return FReply::Unhandled();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
