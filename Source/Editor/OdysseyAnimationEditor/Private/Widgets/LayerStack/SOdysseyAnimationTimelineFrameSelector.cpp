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
	mOnBuildContextMenu = InArgs._OnBuildContextMenu;
	mOnMapActions = InArgs._OnMapActions;

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(20.f)
		[
			//use this scrollbox to display other widgets in the FrameSelector area
			//for now there is no wodgets to display, but I can clearly imagine some
			SNew(SOdysseyAnimationTimelineScrollBox, iEditor)
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
	float offset = mEditor->Timeline()->GetOffset();
	const float frameSize = mEditor->Timeline()->GetFrameWidth();

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

		int timelineOffset = mEditor->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mEditor->Timeline()->GetFrameWidth();
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
		int timelineOffset = mEditor->Timeline()->GetOffset();
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frameWidth = mEditor->Timeline()->GetFrameWidth();
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
SOdysseyAnimationTimelineFrameSelector::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& iEvent)
{
	if (mIsSelecting)
	{
		mIsSelecting = false;
		mEditor->Timeline()->SetSelectedFrames(mSelectionData.mSelectedFrames);
		return FReply::Handled().ReleaseMouseCapture();
	}
	/* else if (iEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
		TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
		mOnMapActions.ExecuteIfBound(commandList);

		FMenuBuilder menuBuilder(true, commandList);
		mOnBuildContextMenu.ExecuteIfBound(menuBuilder);

		TSharedRef<SWidget> menuContents = menuBuilder.MakeWidget();
		FWidgetPath widgetPath = iEvent.GetEventPath() != nullptr ? *iEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), widgetPath, menuContents, iEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
    	return FReply::Handled();
	} */
	
	return FReply::Unhandled();
}

bool
SOdysseyAnimationTimelineFrameSelector::GetSelectedFrames(int& oStartFrame, int& oEndFrame) const
{
	bool isLowerClosed = mEditor->Timeline()->GetSelectedFrames().GetLowerBound().IsClosed();
	bool isUpperClosed = mEditor->Timeline()->GetSelectedFrames().GetUpperBound().IsClosed();
	if (!mIsSelecting && (!isLowerClosed || !isUpperClosed))
		return false;

	oStartFrame = mIsSelecting ? mSelectionData.mSelectedFrames.GetLowerBoundValue() : mEditor->Timeline()->GetSelectedFrames().GetLowerBoundValue();
	oEndFrame = mIsSelecting ? mSelectionData.mSelectedFrames.GetUpperBoundValue() : mEditor->Timeline()->GetSelectedFrames().GetUpperBoundValue();
	return true;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
