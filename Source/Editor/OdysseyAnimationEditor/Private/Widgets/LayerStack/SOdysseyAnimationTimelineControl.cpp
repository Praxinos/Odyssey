// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineCurrentFrame.h"

SOdysseyAnimationTimelineControl::SOdysseyAnimationTimelineControl()
	: mEditor(nullptr)
	, mOffsetMousePosition(0)
	, mIsOffsetting(false)
{
}

void
SOdysseyAnimationTimelineControl::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditor* iEditor
)
{
    mEditor = iEditor;
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			iArgs._Content.Widget
		]
		+ SOverlay::Slot()
		[
			SNew(SOdysseyAnimationTimelineCurrentFrame, mEditor)
		]
	];
}

FReply
SOdysseyAnimationTimelineControl::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		if (MouseEvent.GetWheelDelta() > 0.f)
		{
			mEditor->Timeline()->ZoomOut();
		}
		else
		{
			mEditor->Timeline()->ZoomIn();
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply 
SOdysseyAnimationTimelineControl::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (MouseEvent.IsControlDown())
		{
			mIsOffsetting = true;
			mOffsetMousePosition = MouseEvent.GetScreenSpacePosition();
			mOffsetMousePosition.Y = mEditor->Timeline()->GetOffset();
    		return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
		}
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineControl::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		const float minOffset = 0.0f;
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
        //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		mEditor->Timeline()->SetOffset(FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / mEditor->Timeline()->GetFrameWidth())));
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineControl::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		mIsOffsetting = false;
    	return FReply::Handled().ReleaseMouseCapture();
	}
    return FReply::Unhandled();
}
