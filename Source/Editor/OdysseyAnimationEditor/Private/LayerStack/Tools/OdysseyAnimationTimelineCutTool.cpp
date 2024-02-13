// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineCutTool.h"

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
    if (iParams.mLayer->GetIsLocked())
        return FReply::Unhandled();

	if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
		return FReply::Unhandled();

    float timelineOffset = mTimelineParams->GetOffset();
    float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
    float frameWidth = mTimelineParams->GetFrameWidth();
    float frame = (int)(posX / frameWidth + timelineOffset + 0.5f);

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = iParams.mLayer->GetCellsContainer();
    int cellFrame = cellsContainer->GetCellFrameAtFrame(frame);
    if (cellFrame == INDEX_NONE || cellFrame == 0)
        return FReply::Unhandled();

#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline.move-tool.transaction.set-offset", "Change Layer Offset"));
#endif
    TSharedRef<FOdysseyAnimationCellsMutator> cellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(iParams.mLayer, iParams.mLayer->GetCellsContainer().ToSharedRef());
    cellsMutator->BreakCellAtFrame(frame);

    return FReply::Unhandled();
}
