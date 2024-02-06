// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineSelectionTool.h"

FOdysseyAnimationTimelineSelectionTool::~FOdysseyAnimationTimelineSelectionTool()
{
}

FOdysseyAnimationTimelineSelectionTool::FOdysseyAnimationTimelineSelectionTool(FOdysseyAnimationEditorTimeline* iTimelineParams)
    : mTimelineParams(iTimelineParams)
{   
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
	if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if (iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        //startDragDetection
        mIsSelecting = true;

		float timelineOffset = mTimelineParams->GetOffset();
		float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mTimelineParams->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		mSelectionData.mCursorFrame = frame;
		mSelectionData.mIsDragDetected = false;
        return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
    }
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnDragDetected(const FMouseEventParams& iParams)
{
    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if (mIsSelecting)
  	{
		mSelectionData.mIsDragDetected = true;

		float timelineOffset = mTimelineParams->GetOffset();
		float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mTimelineParams->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

        FInt32Range selectedFrames;
		if (frame >= mSelectionData.mCursorFrame)
		{
			selectedFrames = FInt32Range::Inclusive(mSelectionData.mCursorFrame, frame);
		}
		else
		{
			selectedFrames = FInt32Range::Inclusive(frame, mSelectionData.mCursorFrame);
		}
		
		selectedFrames = FInt32Range::Intersection(selectedFrames, mTimelineParams->GetSelectableFrames());

        mTimelineParams->SetSelectedFrames(selectedFrames);
		return FReply::Handled().CaptureMouse( iParams.mWidget.ToSharedRef() ).PreventThrottling();
  	}

    //startSelection
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseMove(const FMouseEventParams& iParams)
{
    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    if(mIsSelecting)
	{
		if (!mSelectionData.mIsDragDetected)
			return FReply::Unhandled();

		float timelineOffset = mTimelineParams->GetOffset();
		float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mTimelineParams->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);
        
        FInt32Range selectedFrames;
		if (frame < mSelectionData.mCursorFrame)
		{
			selectedFrames = FInt32Range::Inclusive(frame, mSelectionData.mCursorFrame);
		}
		else
		{
			selectedFrames = FInt32Range::Inclusive(mSelectionData.mCursorFrame, frame);
		}

		selectedFrames = FInt32Range::Intersection(selectedFrames, mTimelineParams->GetSelectableFrames());
        
        mTimelineParams->SetSelectedFrames(selectedFrames);
		//mOnSelectionChanged.ExecuteIfBound(mSelectionData.mSelectedFrames);

		return FReply::Handled();
	}

    //updateSelection
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

	if (mIsSelecting)
	{
		mIsSelecting = false;

		if (!mSelectionData.mIsDragDetected)
		{
			FInt32Range selectedFrames = FInt32Range::Empty();
			if (mTimelineParams->GetSelectedFrames().IsEmpty())
			{
				selectedFrames = FInt32Range::Inclusive(mSelectionData.mCursorFrame, mSelectionData.mCursorFrame);
				selectedFrames = FInt32Range::Intersection(selectedFrames, mTimelineParams->GetSelectableFrames());	
			}
			
			mTimelineParams->SetSelectedFrames(selectedFrames);
		}

		float timelineOffset = mTimelineParams->GetOffset();
		float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mTimelineParams->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		mSelectionData.mIsDragDetected = false;
		//mOnSelectionEnded.ExecuteIfBound(frame);
		return FReply::Handled().ReleaseMouseCapture();
	}

    //validateSelection
    return FReply::Unhandled();
}
