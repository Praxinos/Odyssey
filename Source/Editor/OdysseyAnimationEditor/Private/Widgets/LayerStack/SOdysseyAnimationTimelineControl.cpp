// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineCurrentFrame.h"

SOdysseyAnimationTimelineControl::SOdysseyAnimationTimelineControl()
	: mExtension(nullptr)
	, mOffsetMousePosition(0)
	, mIsOffsetting(false)
	, mIsZooming(false)
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
	if (FOdysseyKeyState::GetLastKey() == FKey())
        return FReply::Unhandled();

    FModifierKeysState modifierKeysState = FSlateApplication::Get().GetModifierKeys();
    const FInputChord activeChord(FOdysseyKeyState::GetLastKey(),
        EModifierKey::FromBools(
            modifierKeysState.IsControlDown(),
            modifierKeysState.IsAltDown(),
            modifierKeysState.IsShiftDown(),
            modifierKeysState.IsCommandDown()
        )
    );

    if (FOdysseyAnimationEditorCommands::Get().PanZoomTimeline->HasActiveChord(activeChord))
    {
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			mIsOffsetting = true;
			mOffsetMousePosition = MouseEvent.GetScreenSpacePosition();
			mOffsetMousePosition.Y = mExtension->Timeline()->GetOffset();
    		return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			mIsZooming = true;
			mZoomMousePosition = MouseEvent.GetScreenSpacePosition();
			mZoomMousePosition.Y = mExtension->Timeline()->GetZoom();
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

	if (mIsZooming)
	{
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mZoomMousePosition.X;
        //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		
		//float minZoom = FMath::Loge(mExtension->Timeline()->GetMinZoom());
		//float maxZoom = FMath::Loge(mExtension->Timeline()->GetMaxZoom());
		//float zoom = FMath::Loge(mZoomMousePosition.Y) * (1.f + mouseOffset / 1000.f);
		//mExtension->Timeline()->SetZoom(FMath::Exp(zoom));

		double sliderPos = FMath::Loge(mZoomMousePosition.Y);
		sliderPos += mouseOffset / 200.f;
		double newZoom = FMath::Exp(sliderPos);
		mExtension->Timeline()->SetZoom(newZoom);

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

	if (mIsZooming)
	{
		mIsZooming = false;
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
