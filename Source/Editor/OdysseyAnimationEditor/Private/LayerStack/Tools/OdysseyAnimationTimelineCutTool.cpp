// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineCutTool.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimationEditorTimeline.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCutTool::~FOdysseyAnimationTimelineCutTool()
{
}

FOdysseyAnimationTimelineCutTool::FOdysseyAnimationTimelineCutTool(FOdysseyAnimationEditorTimeline* iTimelineParams)
    : mTimelineParams(iTimelineParams)
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
    int frame = (int)(mTimelineParams->MousePositionToFrame(posX) + 0.5f);

    UOdysseyAnimationCell* cell = iParams.mLayer->GetCellAtFrame(frame);
    if (!cell || cell->GetFrameRange().GetLowerBoundValue() == frame)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.cut-tool.transaction.break-cell", "Break Cell"));
#endif
	cell->Break(frame - cell->GetFrameRange().GetLowerBoundValue());
    return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
