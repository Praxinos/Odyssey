// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineCurrentFrame.h"

SOdysseyAnimationTimelineControl::SOdysseyAnimationTimelineControl()
	: mExtension(nullptr)
	, mOffsetMousePosition(0)
	, mIsOffsetting(false)
{
}

void
SOdysseyAnimationTimelineControl::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditorExtension* iExtension
)
{
    mExtension = iExtension;
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			iArgs._Content.Widget
		]
		+ SOverlay::Slot()
		[
			SNew(SOdysseyAnimationTimelineCurrentFrame, mExtension)
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
			mExtension->Timeline()->ZoomOut();
		}
		else
		{
			mExtension->Timeline()->ZoomIn();
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
			mOffsetMousePosition.Y = mExtension->Timeline()->GetOffset();
    		return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
		}
	}

	return FReply::Unhandled();
}

FReply 
SOdysseyAnimationTimelineControl::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	//Take the focus
	return FReply::Handled().SetUserFocus(AsShared());
}

FReply
SOdysseyAnimationTimelineControl::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		const float minOffset = 0.0f;
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
        //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		mExtension->Timeline()->SetOffset(FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / mExtension->Timeline()->GetFrameWidth())));
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

FNavigationReply
SOdysseyAnimationTimelineControl::OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent)
{
    if (InNavigationEvent.GetNavigationType() == EUINavigation::Left)
    {
        int frame = FMath::Max(0, mExtension->Animation()->CurrentFrame - 1);
        FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), "CurrentFrame", frame);
    }
    else if (InNavigationEvent.GetNavigationType() == EUINavigation::Right)
    {
        int frame = mExtension->Animation()->CurrentFrame + 1;
        FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), "CurrentFrame", frame);
    }
	return FNavigationReply::Stop();
}
