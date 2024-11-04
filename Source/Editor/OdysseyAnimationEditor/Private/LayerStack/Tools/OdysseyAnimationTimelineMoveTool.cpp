// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineMoveTool.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimationEditorTimelinePosition.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineMoveTool::~FOdysseyAnimationTimelineMoveTool()
{
}

FOdysseyAnimationTimelineMoveTool::FOdysseyAnimationTimelineMoveTool(TSharedRef<FOdysseyAnimationEditorTimelinePosition> iTimelinePosition)
    : mTimelinePosition(iTimelinePosition)
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
        mLayerOffsetData.mInitialOffset = iParams.mLayer->CellsOffset;

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

        FOdysseyObjectEditorUtils::SetPropertyValue(iParams.mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), FMath::Max(minOffset, offset), EPropertyChangeType::Interactive);

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

        FOdysseyObjectEditorUtils::SetPropertyValue(iParams.mLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), iParams.mLayer->CellsOffset, EPropertyChangeType::ValueSet);
        #ifdef WITH_EDITOR
            GEditor->EndTransaction();
        #endif
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE
