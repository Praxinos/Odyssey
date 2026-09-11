// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "TimelineTools/OdysseyAnimationTimelineMoveTool.h"

#include "Editor.h"

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
    if (!iParams.mLayer->IsEditable())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if (iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mOffsettingLayer = true;
        mLayerOffsetData.mIsDragDetected = false;
        mLayerOffsetData.mMousePosition = iParams.mMouseEvent.GetScreenSpacePosition();
        mLayerOffsetData.mInitialOffset = iParams.mLayer->GetCellsOffset();

        return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnDragDetected(const FMouseEventParams& iParams)
{
    if (!iParams.mLayer->IsEditable())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if (mOffsettingLayer)
      {
        mLayerOffsetData.mIsDragDetected = true;
    #if WITH_EDITOR
        GEditor->BeginTransaction(LOCTEXT("timeline.move-tool.transaction.set-offset", "Change Layer Offset"));
    #endif

        return FReply::Handled().CaptureMouse(iParams.mWidget.ToSharedRef()).PreventThrottling();
      }
      return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineMoveTool::OnMouseMove(const FMouseEventParams& iParams)
{
    if (!iParams.mLayer->IsEditable())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if ( mOffsettingLayer && mLayerOffsetData.mIsDragDetected)
    {
        const int minOffset = 0;
        float mouseOffset = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition() - mLayerOffsetData.mMousePosition + iParams.mGeometry.GetAbsolutePosition()).X;
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
    if (!iParams.mLayer->IsEditable())
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    //validateMove
    if ( mOffsettingLayer && mLayerOffsetData.mIsDragDetected)
    {
        mLayerOffsetData.mIsDragDetected = false;
        mOffsettingLayer = false;

        iParams.mLayer->SetCellsOffset(iParams.mLayer->GetCellsOffset());
        #if WITH_EDITOR
            GEditor->EndTransaction();
        #endif
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

FMouseCursor
FOdysseyAnimationTimelineMoveTool::GetMouseCursor() const //override
{
    return EMouseCursor::GrabHand;
}

#undef LOCTEXT_NAMESPACE
