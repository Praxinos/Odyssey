// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineFrameSelector.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationTimelineFrameSelector"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineFrameSelector

void
SOdysseyAnimationTimelineFrameSelector::Construct(
	const FArguments& InArgs,
	FOdysseyAnimationEditor* iEditor
)
{
	mEditor = iEditor;

	ChildSlot
	[
		//use this scrollbox to display other widgets in the FrameSelector area
		//for now there is no wodgets to display, but I can clearly imagine some
		SNew(SOdysseyAnimationTimelineScrollBox, iEditor)
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
	float offset = mEditor->Timeline()->GetOffset();
	const float frameSize = mEditor->Timeline()->GetFrameWidth();

	FLinearColor lineColor = FLinearColor::Green;
	lineColor.A = 0.3f;

	int firstFrame = FMath::Min(mSelectionData.mStartFrame, mSelectionData.mEndFrame);
	int lastFrame = FMath::Max(mSelectionData.mStartFrame, mSelectionData.mEndFrame);

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

	return LayerId;
}

FReply 
SOdysseyAnimationTimelineFrameSelector::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		mIsSelecting = true;

		int timelineOffset = mEditor->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mEditor->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		mSelectionData.mStartFrame = frame;
		mSelectionData.mEndFrame = frame;

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
		int timelineOffset = mEditor->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mEditor->Timeline()->GetFrameWidth();
		float frame = (int)(posX / frameWidth + timelineOffset);

		mSelectionData.mEndFrame = frame;
		
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineFrameSelector::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsSelecting)
	{
		int firstFrame = FMath::Min(mSelectionData.mStartFrame, mSelectionData.mEndFrame);
		int lastFrame = FMath::Max(mSelectionData.mStartFrame, mSelectionData.mEndFrame);

		FInt32Range range = FInt32Range::Inclusive(firstFrame, lastFrame);
		//mEditor->Timeline()->SetSelectedFrameRange(range);
		mIsSelecting = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
