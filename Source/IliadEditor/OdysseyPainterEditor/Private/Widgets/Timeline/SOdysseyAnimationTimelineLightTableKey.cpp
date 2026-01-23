// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationTimelineLighttableKey.h"
#include "OdysseyLighttable.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyAnimationTimelineSection.h"

void
SOdysseyAnimationTimelineLighttableKey::Construct(const FArguments& InArgs)
{
    mCell = InArgs._Cell;
    mKey = InArgs._Key;

    const FCheckBoxStyle* checkboxStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("OdysseyCheckBoxStyle.ToggleButton");

    ChildSlot
    [
        SNew(SOdysseyAnimationTimelineSection)
        .TimelinePosition(InArgs._TimelinePosition)
        .WidthInFrames(1)
        .HAlign(HAlign_Center)
        [
            SNew(SOdysseyAnimationTimelineLighttableKeySlider)
            .Key(mKey)
            .OnChanged(InArgs._OnChanged)
            .OnCommited(InArgs._OnCommited)
        ]
    ];
}

void
SOdysseyAnimationTimelineLighttableKeySlider::Construct(const FArguments& InArgs)
{
    mKey = InArgs._Key;
    mOnChanged = InArgs._OnChanged;
    mOnCommited = InArgs._OnCommited;
}

int32
SOdysseyAnimationTimelineLighttableKeySlider::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;

    //Dragging Zone
    const FSlateBrush* backBrush = FOdysseyStyle::GetBrush( "Animation.LighttableKey.Back" );
    const FSlateBrush* frontBrush = FOdysseyStyle::GetBrush( "Animation.LighttableKey.Front" );
    FSlateColor primary( FStyleColors::Primary );
    FSlateColor background( FStyleColors::Background );
    FLinearColor backColor = FOdysseyStyle::GetColor( "Animation.LighttableKey.BackColor" );
    FLinearColor frontColor = FOdysseyStyle::GetColor( "Animation.LighttableKey.FrontColor" );
    FOdysseyLighttableKey key = mKey.Get();

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry( FVector2D(width, height), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(0.f, 0.f) ) ) ),
        backBrush,
        key.bIsActivated ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
        background.GetSpecifiedColor()
    );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry( FVector2D(width, height * key.Opacity / 100.f), FSlateLayoutTransform( 1.0, TransformPoint( 1.0, FVector2D(0.f, height * (1.0f - key.Opacity / 100.f)) ) ) ),
        frontBrush,
        key.bIsActivated ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect,
        primary.GetSpecifiedColor()
    );

    // Draw a current frame
    return LayerId;
}

FReply
SOdysseyAnimationTimelineLighttableKeySlider::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton).PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineLighttableKeySlider::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    mDraggingPosition = iMouseEvent.GetScreenSpacePosition().Y;
    mOldOpacity = mKey.Get().Opacity;
    mDragging = true;
    return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
}

FReply
SOdysseyAnimationTimelineLighttableKeySlider::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mDragging)
    {
        float position = iMouseEvent.GetScreenSpacePosition().Y;
        float delta = (position - mDraggingPosition) * -1;
        float sensitivity = 200.f;

        FOdysseyLighttableKey key = mKey.Get();
        key.Opacity = FMath::Clamp(mOldOpacity + 100.f * delta / sensitivity, 0.f, 100.f);
        mOnChanged.ExecuteIfBound(key);

        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled().PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineLighttableKeySlider::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mDragging)
    {
        //End Mutation
        mOnCommited.ExecuteIfBound(mKey.Get());
        mDragging = false;
        return FReply::Handled().ReleaseMouseCapture();
    }

    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FOdysseyLighttableKey key = mKey.Get();
        key.bIsActivated = !key.bIsActivated;
        mOnCommited.ExecuteIfBound(key);

        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FVector2D
SOdysseyAnimationTimelineLighttableKeySlider::ComputeDesiredSize(float iLayoutScaleMultiplier) const
{
    return FVector2D(16.f, 40.f);
}
