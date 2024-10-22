// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineCurrentFrame.h"
#include "OdysseyKeyState.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"

SOdysseyAnimationTimelineControl::SOdysseyAnimationTimelineControl()
	: mOffsetMousePosition(0)
	, mIsOffsetting(false)
	, mIsZooming(false)
{
}

void
SOdysseyAnimationTimelineControl::Construct(const FArguments& iArgs)
{
    mAnimation = iArgs._Animation;
	mTimelinePosition = iArgs._TimelinePosition;

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			iArgs._Content.Widget
		]
		+ SOverlay::Slot()
		[
			SNew(SOdysseyAnimationTimelineCurrentFrame)
			.CurrentFrame(iArgs._CurrentFrame)
			.TimelinePosition(mTimelinePosition)
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
			mTimelinePosition->ZoomOut();
		}
		else
		{
			mTimelinePosition->ZoomIn();
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
			mOffsetMousePosition.Y = mTimelinePosition->GetOffset();
    		return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			mIsZooming = true;
			mZoomMousePosition = MouseEvent.GetScreenSpacePosition();
			mZoomInitialValue = mTimelinePosition->GetZoom();
			mOffsetInitialValue = mTimelinePosition->GetOffset();
			mInitialValueFrameWidth = mTimelinePosition->GetFrameSize();
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
		mTimelinePosition->SetOffset(mOffsetMousePosition.Y - (mouseOffset / mTimelinePosition->GetFrameSize()));
		return FReply::Handled();
	}

	if (mIsZooming)
	{
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mZoomMousePosition.X;
		float mousePosition = MyGeometry.AbsoluteToLocal(mZoomMousePosition).X;

		float offset = mOffsetInitialValue + mousePosition / mInitialValueFrameWidth;

		double sliderPos = FMath::Loge(mZoomInitialValue);
		sliderPos += mouseOffset / 200.f;
		double newZoom = FMath::Exp(sliderPos);
		mTimelinePosition->SetZoom(newZoom);

		mTimelinePosition->SetOffset(offset - mousePosition / mTimelinePosition->GetFrameSize());

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
        int frame = FMath::Max(0, mAnimation->CurrentFrame - 1);
        FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frame);
    }
    else if (InNavigationEvent.GetNavigationType() == EUINavigation::Right)
    {
        int frame = mAnimation->CurrentFrame + 1;
        FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frame);
    }
	return FNavigationReply::Stop();
}
