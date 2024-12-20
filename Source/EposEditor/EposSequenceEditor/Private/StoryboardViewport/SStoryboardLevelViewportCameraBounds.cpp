// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardViewport/SStoryboardLevelViewportCameraBounds.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "StoryboardViewport/SStoryboardLevelViewport.h"
#include "StoryboardViewport/StoryboardVisibleArea.h"

void SStoryboardLevelViewportCameraBounds::Construct(const FArguments& InArgs, TSharedPtr<SStoryboardLevelViewport> InStoryboardLevelViewport)
{
    StoryboardLevelViewportWeak = InStoryboardLevelViewport;
    SetVisibility(EVisibility::HitTestInvisible);
}

int32 SStoryboardLevelViewportCameraBounds::OnPaint(const FPaintArgs& InPaintArgs, const FGeometry& InAllottedGeometry,
    const FSlateRect& InMyCullingRect, FSlateWindowElementList& OutDrawElements, int32 InLayerId, const FWidgetStyle& InWidgetStyle,
    bool bInParentEnabled) const
{
    InLayerId = SCompoundWidget::OnPaint(InPaintArgs, InAllottedGeometry, InMyCullingRect, OutDrawElements, InLayerId, InWidgetStyle, bInParentEnabled);
    UEposSequenceEditorSettings* settings = GetMutableDefault<UEposSequenceEditorSettings>();
    if (settings)
    {
        DrawCameraBounds(InPaintArgs, InAllottedGeometry, InMyCullingRect, OutDrawElements, InLayerId, settings->ViewportSettings.CameraBoundsShadeColor);
    }

    return InLayerId;
}

void SStoryboardLevelViewportCameraBounds::DrawCameraBounds(const FPaintArgs& InPaintArgs, const FGeometry& InAllottedGeometry,
    const FSlateRect& InMyCullingRect, FSlateWindowElementList& OutDrawElements, int32& InOutLayerId, const FLinearColor& InQuadColor) const
{
    TSharedPtr<SStoryboardLevelViewport> StoryboardLevelViewport = StoryboardLevelViewportWeak.Pin();
    if (!StoryboardLevelViewport.IsValid())
    {
        return;
    }

    TSharedPtr<FStoryboardLevelViewportClient> StoryboardLevelViewportClient = StoryboardLevelViewport->GetViewportClient();
    if (!StoryboardLevelViewportClient.IsValid())
    {
        return;
    }

    ++InOutLayerId;

    const FStoryboardVisibleArea& VisibleArea = StoryboardLevelViewportClient->GetZoomedVisibleArea();
    //const FVector2D Offset = VisibleArea.Offset;

    if (!VisibleArea.IsValid())
    {
        return;
    }

    /*const FVector2D QuadTopTopLeft = VisibleArea.GetVisiblePosition(-VisibleArea.AbsoluteSize) - Offset;
    const FVector2D QuadTopBottomRight = VisibleArea.GetVisiblePosition({VisibleArea.AbsoluteSize.X * 2.f, 0.f}) + FVector2D(Offset.X, 0.f);

    const FVector2D QuadLeftTopLeft = VisibleArea.GetVisiblePosition({-VisibleArea.AbsoluteSize.X, 0.f}) - FVector2D(Offset.X, 0.f);
    const FVector2D QuadLeftBottomRight = VisibleArea.GetVisiblePosition({0.f, VisibleArea.AbsoluteSize.Y});

    const FVector2D QuadRightTopLeft = VisibleArea.GetVisiblePosition({VisibleArea.AbsoluteSize.X, 0.f});
    const FVector2D QuadRightBottomRight = VisibleArea.GetVisiblePosition({VisibleArea.AbsoluteSize.X * 2.f, VisibleArea.AbsoluteSize.Y}) + FVector2D(Offset.X, 0.f);

    const FVector2D QuadBottomTopLeft = VisibleArea.GetVisiblePosition({-VisibleArea.AbsoluteSize.X, VisibleArea.AbsoluteSize.Y}) - FVector2D(Offset.X, 0.f);
    const FVector2D QuadBottomBottomRight = VisibleArea.GetVisiblePosition({VisibleArea.AbsoluteSize.X * 2.f, VisibleArea.AbsoluteSize.Y * 2.f}) + Offset;

    const FPaintGeometry TopRect = InAllottedGeometry.ToPaintGeometry(
        QuadTopBottomRight - QuadTopTopLeft,
        FSlateLayoutTransform(QuadTopTopLeft + Offset)
    );

    const FPaintGeometry LeftRect = InAllottedGeometry.ToPaintGeometry(
        QuadLeftBottomRight - QuadLeftTopLeft,
        FSlateLayoutTransform(QuadLeftTopLeft + Offset)
    );

    const FPaintGeometry RightRect = InAllottedGeometry.ToPaintGeometry(
        QuadRightBottomRight - QuadRightTopLeft,
        FSlateLayoutTransform(QuadRightTopLeft + Offset)
    );

    const FPaintGeometry BottomRect = InAllottedGeometry.ToPaintGeometry(
        QuadBottomBottomRight - QuadBottomTopLeft,
        FSlateLayoutTransform(QuadBottomTopLeft + Offset)
    ); */

    static const FSlateBrush* White = FAppStyle::Get().GetBrush("Brushes.White");

    /*FSlateDrawElement::MakeBox(
        OutDrawElements,
        InOutLayerId,
        InAllottedGeometry.ToPaintGeometry(),
        White,
        ESlateDrawEffect::NoPixelSnapping,
        InQuadColor
    ); */

    const FVector2D CachedViewportSize = StoryboardLevelViewportClient->GetViewportGeometry().WidgetSize;
    if (FMath::IsNearlyZero(CachedViewportSize.X) || FMath::IsNearlyZero(CachedViewportSize.Y))
    {
        return;
    }

    FSlateDrawElement::MakeLines(
        OutDrawElements,
        InOutLayerId,
        InAllottedGeometry.ToPaintGeometry(),
        {
            FVector2f(VisibleArea.TopLeft + FVector2D(CachedViewportSize.X / 2.f, CachedViewportSize.Y / 2.f)),
            FVector2f(VisibleArea.TopRight + FVector2D(CachedViewportSize.X / 2.f, CachedViewportSize.Y / 2.f)),
            FVector2f(VisibleArea.BottomRight + FVector2D(CachedViewportSize.X / 2.f, CachedViewportSize.Y / 2.f)),
            FVector2f(VisibleArea.BottomLeft + FVector2D(CachedViewportSize.X / 2.f, CachedViewportSize.Y / 2.f)),
            FVector2f(VisibleArea.TopLeft + FVector2D(CachedViewportSize.X / 2.f, CachedViewportSize.Y / 2.f))
        },
        ESlateDrawEffect::NoPixelSnapping,
        FLinearColor::Red,
        true,
        2.f
    );

    /*FSlateDrawElement::MakeBox(
        OutDrawElements,
        InOutLayerId,
        TopRect,
        White,
        ESlateDrawEffect::NoPixelSnapping,
        InQuadColor
    );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        InOutLayerId,
        LeftRect,
        White,
        ESlateDrawEffect::NoPixelSnapping,
        InQuadColor
    );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        InOutLayerId,
        RightRect,
        White,
        ESlateDrawEffect::NoPixelSnapping,
        InQuadColor
    );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        InOutLayerId,
        BottomRect,
        White,
        ESlateDrawEffect::NoPixelSnapping,
        InQuadColor
    );*/
}
