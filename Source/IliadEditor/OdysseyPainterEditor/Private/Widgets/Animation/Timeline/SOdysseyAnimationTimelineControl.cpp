// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineControl.h"
#include "OdysseyKeyState.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimation.h"
#include "OdysseyStyle.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"

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

    SetClipping(EWidgetClipping::ClipToBoundsAlways);

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

    FLinearColor outOfRangeColor = FOdysseyStyle::Get().GetSlateColor("Animation.Timeline.OutOfRangeColor").GetSpecifiedColor();
    outOfRangeColor.A = FOdysseyStyle::Get().GetFloat("Animation.Timeline.OutOfRangeColorOpacity");

    FLinearColor outOfBoundColor = FOdysseyStyle::Get().GetSlateColor("Animation.Timeline.OutOfBoundColor").GetSpecifiedColor();
    outOfBoundColor.A = FOdysseyStyle::Get().GetFloat("Animation.Timeline.OutOfBoundColorOpacity");

    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;
    const float frameSize = mTimelinePosition->GetFrameSize();

    FLinearColor lineColor = FLinearColor::Red;
    lineColor.A = 0.3f;

    int currentFrame = mCurrentFrame.Get();
    float currentFramePos = FrameToMousePosition(currentFrame);

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
        float leftRangeX = FrameToMousePosition(validRange.GetLowerBoundValue());
        float rightRangeX = FrameToMousePosition(validRange.GetUpperBoundValue() + 1);

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

    int leftBoundFrame = mAnimation->GetLeftBoundValue();
    int rightBoundFrame = mAnimation->GetRightBoundValue();

    float leftBoundX = FrameToMousePosition(leftBoundFrame);
    float rightBoundX = FrameToMousePosition(rightBoundFrame + 1);

    if ( leftBoundX > 0.f )
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(FVector2D(leftBoundX, height), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(0.f, 0.f)))),
            GenericBrush,
            ESlateDrawEffect::None,
            outOfBoundColor
        );
    }

    if ( rightBoundX < width )
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(FVector2D(width - rightBoundX, height), FSlateLayoutTransform(1.0, TransformPoint(1.0, FVector2D(rightBoundX, 0.f)))),
            GenericBrush,
            ESlateDrawEffect::None,
            outOfBoundColor
        );
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

    if (FOdysseyPainterEditorAnimationCommands::Get().PanZoomTimeline->HasActiveChord(activeChord))
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
        mTimelinePosition->SetOffset(FMath::Max(0, mOffsetMousePosition.Y - (mouseOffset / mTimelinePosition->GetFrameSize())));
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
        mTimelinePosition->SetOffset(FMath::Max(0, offset - mousePosition / mTimelinePosition->GetFrameSize()));

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

float
SOdysseyAnimationTimelineControl::MousePositionToFrame(float iX) const
{
    return (iX - mTimelinePosition->GetPadding() + mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize()) / mTimelinePosition->GetFrameSize();
}

float
SOdysseyAnimationTimelineControl::FrameToMousePosition(float iFrame) const
{
    return iFrame * mTimelinePosition->GetFrameSize() + mTimelinePosition->GetPadding() - mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize();
}
