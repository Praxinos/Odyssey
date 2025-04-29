// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TimelineTools/OdysseyAnimationTimelineMoveTool.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineMoveTool::~FOdysseyAnimationTimelineMoveTool()
{
}

FOdysseyAnimationTimelineMoveTool::FOdysseyAnimationTimelineMoveTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition)
    : FOdysseyAnimationTimelineTool(iTimelinePosition)
{
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->IsLockedRecursively())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if (iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mOffsettingLayer = true;
        mLayerOffsetData.mIsDragDetected = false;
        mLayerOffsetData.mMousePosition = iParams.mMouseEvent.GetScreenSpacePosition().X;
        mLayerOffsetData.mInitialOffset = iParams.mLayer->GetCellsOffset();

        return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnDragDetected(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->IsLockedRecursively())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if (mOffsettingLayer)
      {
        mLayerOffsetData.mIsDragDetected = true;
    #ifdef WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.move-tool.transaction.set-offset", "Change Layer Offset"));
    #endif

        return FReply::Handled().CaptureMouse(iParams.mWidget.ToSharedRef()).PreventThrottling();
      }
      return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseMove(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->IsLockedRecursively())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if ( mOffsettingLayer && mLayerOffsetData.mIsDragDetected)
    {
        const int minOffset = 0;
        float mouseOffset = iParams.mMouseEvent.GetScreenSpacePosition().X - mLayerOffsetData.mMousePosition;
        int offset = (int)(mLayerOffsetData.mInitialOffset + (mouseOffset / mTimelinePosition->GetFrameSize()));

        iParams.mLayer->SetCellsOffsetInteractive(FMath::Max(minOffset, offset));

        return FReply::Handled();
    }

    //updateMove
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mLayer->IsLockedRecursively())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    //validateMove
    if ( mOffsettingLayer && mLayerOffsetData.mIsDragDetected)
    {
        mLayerOffsetData.mIsDragDetected = false;
        mOffsettingLayer = false;

        iParams.mLayer->SetCellsOffset(iParams.mLayer->GetCellsOffset());
        #ifdef WITH_EDITOR
            GEditor->EndTransaction();
        #endif
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
