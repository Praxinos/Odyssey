// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineMoveTool.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "OdysseyAnimationEditorTimeline.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineMoveTool::~FOdysseyAnimationTimelineMoveTool()
{
}

FOdysseyAnimationTimelineMoveTool::FOdysseyAnimationTimelineMoveTool(FOdysseyAnimationEditorTimeline* iTimelineParams)
    : mTimelineParams(iTimelineParams)
    , mCellsMutator(nullptr)
{   
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->GetIsLocked())
        return FReply::Unhandled();

	if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
		return FReply::Unhandled();

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = iParams.mLayer->GetCellsContainer();

	if (iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mOffsettingLayer = true;
        mLayerOffsetData.mIsDragDetected = false;
        mLayerOffsetData.mMousePosition = iParams.mMouseEvent.GetScreenSpacePosition().X;
        mLayerOffsetData.mInitialOffset = cellsContainer->GetOffset();

        return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnDragDetected(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->GetIsLocked())
        return FReply::Unhandled();

	if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
		return FReply::Unhandled();

    if (mOffsettingLayer)
  	{
        mLayerOffsetData.mIsDragDetected = true;
        mCellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(iParams.mLayer, iParams.mLayer->GetCellsContainer().ToSharedRef());
        return FReply::Handled().CaptureMouse(iParams.mWidget.ToSharedRef()).PreventThrottling();
  	}
  	return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseMove(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->GetIsLocked())
        return FReply::Unhandled();

	if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
		return FReply::Unhandled();
		
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = iParams.mLayer->GetCellsContainer();

    if ( mOffsettingLayer && mLayerOffsetData.mIsDragDetected)
    {
        const int minOffset = 0;
        float mouseOffset = iParams.mMouseEvent.GetScreenSpacePosition().X - mLayerOffsetData.mMousePosition;
        int offset = (int)(mLayerOffsetData.mInitialOffset + (mouseOffset / mTimelineParams->GetFrameWidth()));
        mCellsMutator->SetOffset(FMath::Max(minOffset, offset));

        return FReply::Handled();
    }
	
    //updateMove
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->GetIsLocked())
        return FReply::Unhandled();

	if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
		return FReply::Unhandled();

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = iParams.mLayer->GetCellsContainer();

    //validateMove
	if ( mOffsettingLayer && mLayerOffsetData.mIsDragDetected)
    {
        mLayerOffsetData.mIsDragDetected = false;

#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline.move-tool.transaction.set-offset", "Change Layer Offset"));
#endif
        mCellsMutator->Commit();
        mOffsettingLayer = false;
        mCellsMutator = nullptr;

        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
