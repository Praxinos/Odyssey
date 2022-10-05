// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyTimelineFrameHandle.h"

#include "Fonts/FontMeasure.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

void SOdysseyTimelineFrameHandle::Construct( const SOdysseyTimelineFrameHandle::FArguments& InArgs )
{
    mFrameSize = InArgs._FrameSize;
	mIsDragging = false;
	mOnDragStarted = InArgs._OnDragStarted;
	mOnDragged = InArgs._OnDragged;
	mOnDragStopped = InArgs._OnDragStopped;

	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		InArgs._Content.Widget
	];
}

FReply
SOdysseyTimelineFrameHandle::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		mDragScreenSpacePosition = iMouseEvent.GetScreenSpacePosition();
		mIsDragging = true;
		mOnDragStarted.ExecuteIfBound();
		return FReply::Handled().CaptureMouse(this->AsShared());	
	}
	return FReply::Unhandled();
}

FReply
SOdysseyTimelineFrameHandle::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (mIsDragging)
	{
		float offset = iMouseEvent.GetScreenSpacePosition().X - mDragScreenSpacePosition.X;
		int32 frameOffset = FGenericPlatformMath::RoundToInt(offset / mFrameSize.Get());
		mOnDragged.ExecuteIfBound(frameOffset);
		return FReply::Handled();	
	}
	return FReply::Unhandled();
}

FReply
SOdysseyTimelineFrameHandle::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (mIsDragging)
	{
		mIsDragging = false;
		mOnDragStopped.ExecuteIfBound();
		return FReply::Handled().ReleaseMouseCapture();	
	}
	return FReply::Unhandled();
}


float
SOdysseyTimelineFrameHandle::FrameSize() const
{
	return mFrameSize.Get();
}

#undef LOCTEXT_NAMESPACE