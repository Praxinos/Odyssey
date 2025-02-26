// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineLightTableKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyStyle.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationEditorExtension.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyAnimationTimelineSection.h"

void
SOdysseyAnimationTimelineLightTableKey::Construct(const FArguments& InArgs)
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
            SNew(SOdysseyAnimationTimelineLightTableKeySlider)
            .Key(mKey)
            .OnChanged(InArgs._OnChanged)
            .OnCommited(InArgs._OnCommited)
        ]
    ];
}

void
SOdysseyAnimationTimelineLightTableKeySlider::Construct(const FArguments& InArgs)
{
    mKey = InArgs._Key;
    mOnChanged = InArgs._OnChanged;
    mOnCommited = InArgs._OnCommited;
}

int32
SOdysseyAnimationTimelineLightTableKeySlider::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

    const float height = AllottedGeometry.GetLocalSize().Y;
    const float width = AllottedGeometry.GetLocalSize().X;

    //Dragging Zone
    const FSlateBrush* backBrush = FOdysseyStyle::GetBrush( "Animation.LightTableKey.Back" );
    const FSlateBrush* frontBrush = FOdysseyStyle::GetBrush( "Animation.LightTableKey.Front" );
    FSlateColor primary( FStyleColors::Primary );
    FSlateColor background( FStyleColors::Background );
    FLinearColor backColor = FOdysseyStyle::GetColor( "Animation.LightTableKey.BackColor" );
    FLinearColor frontColor = FOdysseyStyle::GetColor( "Animation.LightTableKey.FrontColor" );
    FOdysseyAnimationLightTableKey key = mKey.Get();

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
SOdysseyAnimationTimelineLightTableKeySlider::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton).PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineLightTableKeySlider::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    mDraggingPosition = iMouseEvent.GetScreenSpacePosition().Y;
    mOldOpacity = mKey.Get().Opacity;
    mDragging = true;
    return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
}

FReply
SOdysseyAnimationTimelineLightTableKeySlider::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mDragging)
    {
        float position = iMouseEvent.GetScreenSpacePosition().Y;
        float delta = (position - mDraggingPosition) * -1;
        float sensitivity = 200.f;

        FOdysseyAnimationLightTableKey key = mKey.Get();
        key.Opacity = FMath::Clamp(mOldOpacity + 100.f * delta / sensitivity, 0.f, 100.f);
        mOnChanged.ExecuteIfBound(key);

        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled().PreventThrottling();
    }

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineLightTableKeySlider::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
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
        FOdysseyAnimationLightTableKey key = mKey.Get();
        key.bIsActivated = !key.bIsActivated;
        mOnCommited.ExecuteIfBound(key);

        // This has prevent throttling on so that viewports continue to run whilst dragging the slider
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FVector2D
SOdysseyAnimationTimelineLightTableKeySlider::ComputeDesiredSize(float iLayoutScaleMultiplier) const
{
    return FVector2D(16.f, 40.f);
}
