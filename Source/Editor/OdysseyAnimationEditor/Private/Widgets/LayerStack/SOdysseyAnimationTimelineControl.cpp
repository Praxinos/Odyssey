// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"
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
	mCurrentFrame = iArgs._CurrentFrame;
	mCustomValidRange = iArgs._CustomValidRange;

	ChildSlot
	[
		iArgs._Content.Widget
	];
}

int32
SOdysseyAnimationTimelineControl::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	LayerId++;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");

	const float height = AllottedGeometry.GetLocalSize().Y;
	const float width = AllottedGeometry.GetLocalSize().X;
	const float frameSize = mTimelinePosition->GetFrameSize();

	FLinearColor lineColor = FLinearColor::Red;
	lineColor.A = 0.3f;

	int currentFrame = mCurrentFrame.Get();
	float currentFramePos = mTimelinePosition->FrameToMousePosition(currentFrame);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
        AllottedGeometry.ToPaintGeometry( FVector2D(frameSize, height), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(currentFramePos, 0.f) ) ) ),
		GenericBrush,
		ESlateDrawEffect::None,
		lineColor
	);

	FInt32Range validRange = mCustomValidRange.Get();
	if (!validRange.IsEmpty())
	{	
		FLinearColor outOfRangeColor = FLinearColor::Black;
		outOfRangeColor.A = 0.3f;

		float leftRangeX = FMath::Min(width, mTimelinePosition->FrameToMousePosition(validRange.GetLowerBoundValue()));
		float rightRangeX = FMath::Max(0, mTimelinePosition->FrameToMousePosition(validRange.GetUpperBoundValue() + 1));

		if (leftRangeX > 0.f)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry( FVector2D(leftRangeX, height), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(0.f, 0.f) ) ) ),
				GenericBrush,
				ESlateDrawEffect::None,
				outOfRangeColor
			);
		}

		if (rightRangeX < width)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry( FVector2D(width - rightRangeX, height), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(rightRangeX, 0.f) ) ) ),
				GenericBrush,
				ESlateDrawEffect::None,
				outOfRangeColor
			);
		}
	}

	++LayerId;
	return LayerId;
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
