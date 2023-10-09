// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineFrameSelector.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationTimelineFrameSelector"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineFrameSelector

SOdysseyAnimationTimelineFrameSelector::SOdysseyAnimationTimelineFrameSelector()
	: mCommandList(MakeShared<FUICommandList>())
{
}

void
SOdysseyAnimationTimelineFrameSelector::Construct(
	const FArguments& InArgs,
	FOdysseyAnimationEditorExtension* iExtension
)
{
	mExtension = iExtension;

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(20.f)
		[
			//use this scrollbox to display other widgets in the FrameSelector area
			//for now there is no wodgets to display, but I can clearly imagine some
			SNew(SOdysseyAnimationTimelineScrollBox, iExtension)
		]
	];
}

int32 SOdysseyAnimationTimelineFrameSelector::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
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
	lineColor.A = 0.3f;

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
		mSelectionData.mSelectedFrames = FInt32Range::Inclusive(frame, frame);

		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineFrameSelector::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(mIsSelecting)
	{
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
		mExtension->Timeline()->SetSelectedFrames(mSelectionData.mSelectedFrames);
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

bool
SOdysseyAnimationTimelineFrameSelector::GetSelectedFrames(int& oStartFrame, int& oEndFrame) const
{
	if (!mIsSelecting && mExtension->Timeline()->GetSelectedFrames().IsEmpty())
		return false;

	oStartFrame = mIsSelecting ? mSelectionData.mSelectedFrames.GetLowerBoundValue() : mExtension->Timeline()->GetSelectedFrames().GetLowerBoundValue();
	oEndFrame = mIsSelecting ? mSelectionData.mSelectedFrames.GetUpperBoundValue() : mExtension->Timeline()->GetSelectedFrames().GetUpperBoundValue();
	return true;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
