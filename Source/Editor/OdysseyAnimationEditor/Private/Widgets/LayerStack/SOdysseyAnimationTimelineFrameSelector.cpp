// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineFrameSelector.h"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineFrameSelector

void
SOdysseyAnimationTimelineFrameSelector::Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension)
{
	mExtension = iExtension;
	mSelectableFrames = InArgs._SelectableFrames;
	mOnSelectionStarted = InArgs._OnSelectionStarted;
	mOnSelectionEnded = InArgs._OnSelectionEnded;
	mOnSelectionChanged = InArgs._OnSelectionChanged;

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(15.f)
	];
}

FReply 
SOdysseyAnimationTimelineFrameSelector::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		mIsSelecting = true;

		int timelineOffset = mExtension->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mExtension->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		mSelectionData.mCursorFrame = frame;
		mSelectionData.mIsDragDetected = false;
		
		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}


FReply
SOdysseyAnimationTimelineFrameSelector::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
  	if (mIsSelecting)
  	{
		mSelectionData.mIsDragDetected = true;

		int timelineOffset = mExtension->Timeline()->GetOffset();
		float posX = iGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mExtension->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		if (frame >= mSelectionData.mCursorFrame)
		{
			mSelectionData.mSelectedFrames = FInt32Range::Inclusive(mSelectionData.mCursorFrame, frame);
		}
		else
		{
			mSelectionData.mSelectedFrames = FInt32Range::Inclusive(frame, mSelectionData.mCursorFrame);
		}
		
		mSelectionData.mSelectedFrames = FInt32Range::Intersection(mSelectionData.mSelectedFrames, mSelectableFrames.Get());
		mOnSelectionStarted.ExecuteIfBound(mSelectionData.mCursorFrame);
		mOnSelectionChanged.ExecuteIfBound(mSelectionData.mSelectedFrames);
		return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
  	}
  	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineFrameSelector::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(mIsSelecting)
	{
		if (!mSelectionData.mIsDragDetected)
			return FReply::Unhandled();

		int timelineOffset = mExtension->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mExtension->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		if (frame < mSelectionData.mCursorFrame)
		{
			mSelectionData.mSelectedFrames = FInt32Range::Inclusive(frame, mSelectionData.mCursorFrame);
		}
		else
		{
			mSelectionData.mSelectedFrames = FInt32Range::Inclusive(mSelectionData.mCursorFrame, frame);
		}

		mSelectionData.mSelectedFrames = FInt32Range::Intersection(mSelectionData.mSelectedFrames, mSelectableFrames.Get());
		mOnSelectionChanged.ExecuteIfBound(mSelectionData.mSelectedFrames);

		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineFrameSelector::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iEvent)
{
	if (mIsSelecting)
	{
		mIsSelecting = false;

		if (!mSelectionData.mIsDragDetected)
		{
			mSelectionData.mSelectedFrames = FInt32Range::Empty();
			mOnSelectionChanged.ExecuteIfBound(mSelectionData.mSelectedFrames);
		}

		int timelineOffset = mExtension->Timeline()->GetOffset();
		float posX = iGeometry.AbsoluteToLocal(iEvent.GetScreenSpacePosition()).X;
		float frameWidth = mExtension->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		mSelectionData.mIsDragDetected = false;
		mOnSelectionEnded.ExecuteIfBound(frame);
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineFrameSelection

SOdysseyAnimationTimelineFrameSelection::SOdysseyAnimationTimelineFrameSelection()
{
}

void
SOdysseyAnimationTimelineFrameSelection::Construct(
	const FArguments& InArgs,
	FOdysseyAnimationEditorExtension* iExtension
)
{
	mExtension = iExtension;
	mSelectedFrames = InArgs._SelectedFrames;
	mOnSelectionDragged = InArgs._OnSelectionDragged;

	ChildSlot
	[
		InArgs._Content.Widget
	];
}

int32 SOdysseyAnimationTimelineFrameSelection::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw a current frame
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	++LayerId;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = mExtension->Timeline()->GetOffset();
	const float frameSize = mExtension->Timeline()->GetFrameWidth();

	FLinearColor lineColor = FLinearColor::Green;
	lineColor.A = 0.2f;

	int firstFrame = INDEX_NONE;
	int lastFrame = INDEX_NONE;

	bool bDisplay = GetSelectedFrames(firstFrame, lastFrame);

	if(bDisplay)
	{
		float firstFramePos = (firstFrame - offset) * frameSize;
		float lastFramePos = (lastFrame - offset) * frameSize;
		float selectionSize = lastFramePos - firstFramePos + frameSize;

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(FVector2D(firstFramePos, 0.f), FVector2D(selectionSize, height)),
			GenericBrush,
			ESlateDrawEffect::None,
			lineColor
		);
	}

	return LayerId;
}


bool
SOdysseyAnimationTimelineFrameSelection::GetSelectedFrames(int& oStartFrame, int& oEndFrame) const
{
	/* if (mSelectionData.mIsSelecting)
	{
		oStartFrame = mSelectionData.mSelectedFrames.GetLowerBoundValue();
		oEndFrame = mSelectionData.mSelectedFrames.GetUpperBoundValue();
		return true;
	} */

	FInt32Range selectedFrames = mSelectedFrames.Get();
	if (selectedFrames.IsEmpty())
		return false;

	oStartFrame = selectedFrames.GetLowerBoundValue();
	oEndFrame = selectedFrames.GetUpperBoundValue();
	return true;
}


/* void
SOdysseyAnimationTimelineFrameSelection::OnFrameSelectionChanged(FInt32Range iSelectedFrames)
{
	mSelectionData.mSelectedFrames = iSelectedFrames;
	mOnSelectionChanged.ExecuteIfBound(iSelectedFrames);
}

void
SOdysseyAnimationTimelineFrameSelection::OnFrameSelectionStarted(int iFrame)
{
	mSelectionData.mIsSelecting = true;
	mSelectionData.mSelectedFrames = FInt32Range::Empty();
	mOnSelectionStarted.ExecuteIfBound(iFrame);
}

void
SOdysseyAnimationTimelineFrameSelection::OnFrameSelectionEnded(int iFrame)
{
	mSelectionData.mIsSelecting = false;
	mOnSelectionEnded.ExecuteIfBound(iFrame);
} */

FReply 
SOdysseyAnimationTimelineFrameSelection::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FInt32Range selectedFrames = mSelectedFrames.Get();
		if (selectedFrames.IsEmpty())
			return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

		int timelineOffset = mExtension->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mExtension->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);
		
		if (frame < selectedFrames.GetLowerBoundValue() || frame > selectedFrames.GetUpperBoundValue())
			return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
		
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}


FReply
SOdysseyAnimationTimelineFrameSelection::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (mOnSelectionDragged.IsBound())
		return mOnSelectionDragged.Execute();

	return FReply::Unhandled();
}
