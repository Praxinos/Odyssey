// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "StoryboardViewport/SStoryboardLevelViewportCameraBounds.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "StoryboardViewport/SStoryboardLevelViewport.h"
#include "StoryboardViewport/StoryboardVisibleArea.h"
#include "BoxTypes.h"
#include "Polygon2.h"

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

    if (!VisibleArea.IsValid())
    {
        return;
    }

    static const FSlateBrush* White = FAppStyle::Get().GetBrush("Brushes.White");

    const FVector2D CachedViewportSize = StoryboardLevelViewportClient->GetViewportGeometry().WidgetSize;
    if (FMath::IsNearlyZero(CachedViewportSize.X) || FMath::IsNearlyZero(CachedViewportSize.Y))
    {
        return;
    }

    FVector2D topLeft = VisibleArea.TopLeft;
    FVector2D topRight = VisibleArea.TopRight;
    FVector2D bottomLeft = VisibleArea.BottomLeft;
    FVector2D bottomRight = VisibleArea.BottomRight;

    /* USE for DEBUGGING
        FSlateDrawElement::MakeLines(
        OutDrawElements,
        InOutLayerId++,
        InAllottedGeometry.ToPaintGeometry(),
        {
            FVector2f(topLeft),
            FVector2f(topRight),
            FVector2f(bottomRight),
            FVector2f(bottomLeft),
            FVector2f(topLeft)
        },
        ESlateDrawEffect::NoPixelSnapping,
        FLinearColor::Red,
        true,
        2.f
    ); */

    ::UE::Geometry::FPolygon2d polygon(
        {
            topLeft,
            topRight,
            bottomRight,
            bottomLeft,
        }
    );

    ::UE::Geometry::FAxisAlignedBox2d bounds = polygon.Bounds();

    FVector2D topBoxPosition(0.f, 0.f);
    FVector2D topBoxSize(CachedViewportSize.X, FMath::Min(bounds.Min.Y, CachedViewportSize.Y));
    FVector2D bottomBoxPosition(0.f, bounds.Max.Y);
    FVector2D bottomBoxSize(CachedViewportSize.X, FMath::Max(CachedViewportSize.Y - bounds.Max.Y, 0.f));
    FVector2D leftBoxPosition(0.f, topBoxSize.Y);
    FVector2D leftBoxSize(FMath::Min(bounds.Min.X, CachedViewportSize.X), CachedViewportSize.Y - topBoxSize.Y - bottomBoxSize.Y);
    FVector2D rightBoxPosition(bounds.Max.X, topBoxSize.Y);
    FVector2D rightBoxSize(FMath::Max(CachedViewportSize.X - bounds.Max.X, 0.f), CachedViewportSize.Y - topBoxSize.Y - bottomBoxSize.Y);

    FVector2D topmostPoint = polygon[0];
    FVector2D bottommostPoint = polygon[0];
    FVector2D leftmostPoint = polygon[0];
    FVector2D rightmostPoint = polygon[0];
    for (int i = 1; i <= 3; i++)
    {
        if (polygon[i].Y < topmostPoint.Y || (polygon[i].Y == topmostPoint.Y && polygon[i].X < topmostPoint.X) )
        {
            topmostPoint = polygon[i];
        }
        if (polygon[i].Y > bottommostPoint.Y || (polygon[i].Y == bottommostPoint.Y && polygon[i].X > bottommostPoint.X) )
        {
            bottommostPoint = polygon[i];
        }
        if (polygon[i].X < leftmostPoint.X || (polygon[i].X == leftmostPoint.X && polygon[i].Y > leftmostPoint.Y) )
        {
            leftmostPoint = polygon[i];
        }
        if (polygon[i].X > rightmostPoint.X || (polygon[i].X == rightmostPoint.X && polygon[i].Y < rightmostPoint.Y) )
        {
            rightmostPoint = polygon[i];
        }
    }

    ::UE::Geometry::FPolygon2d topLeftTriangle(
        {
            FVector2D(leftBoxSize.X, topBoxSize.Y),
            topmostPoint,
            leftmostPoint,
        }
    );

    ::UE::Geometry::FPolygon2d topRightTriangle(
        {
            FVector2D(rightBoxPosition.X, topBoxSize.Y),
            rightmostPoint,
            topmostPoint,
        }
    );

    ::UE::Geometry::FPolygon2d bottomRightTriangle(
        {
            FVector2D(rightBoxPosition.X, bottomBoxPosition.Y),
            bottommostPoint,
            rightmostPoint,
        }
    );

    ::UE::Geometry::FPolygon2d bottomLeftTriangle(
        {
            FVector2D(leftBoxSize.X, bottomBoxPosition.Y),
            leftmostPoint,
            bottommostPoint,
        }
    );

    ::UE::Geometry::FAxisAlignedBox2d viewportBox(FVector2D(0.f, 0.f), CachedViewportSize);

    topLeftTriangle.ClipConvex(viewportBox);
    topRightTriangle.ClipConvex(viewportBox);
    bottomRightTriangle.ClipConvex(viewportBox);
    bottomLeftTriangle.ClipConvex(viewportBox);

    if (bounds.Min.Y >= 0.f)
    {

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InOutLayerId,
            InAllottedGeometry.ToPaintGeometry(topBoxSize, FSlateLayoutTransform(topBoxPosition)),
            White,
            ESlateDrawEffect::NoPixelSnapping,
            InQuadColor
        );
    }

    if (bounds.Max.Y < CachedViewportSize.Y)
    {

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InOutLayerId,
            InAllottedGeometry.ToPaintGeometry(bottomBoxSize, FSlateLayoutTransform(bottomBoxPosition)),
            White,
            ESlateDrawEffect::NoPixelSnapping,
            InQuadColor
        );
    }

    if (bounds.Min.X >= 0.f)
    {

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InOutLayerId,
            InAllottedGeometry.ToPaintGeometry(leftBoxSize, FSlateLayoutTransform(leftBoxPosition)),
            White,
            ESlateDrawEffect::NoPixelSnapping,
            InQuadColor
        );
    }

    if (bounds.Max.X < CachedViewportSize.X)
    {

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InOutLayerId,
            InAllottedGeometry.ToPaintGeometry(rightBoxSize, FSlateLayoutTransform(rightBoxPosition)),
            White,
            ESlateDrawEffect::NoPixelSnapping,
            InQuadColor
        );
    }

    if (topLeftTriangle.Area() > 0.f)
    {
        TArray<FSlateVertex> Vertices;
        TArray<SlateIndex> VertexIndices = {0, 1, 2};
        Vertices.AddUninitialized(topLeftTriangle.VertexCount());

        for (int i = 0; i < topLeftTriangle.VertexCount(); ++i)
        {
            Vertices[i].Position = FVector2f(InAllottedGeometry.LocalToAbsolute(topLeftTriangle[i]));
            Vertices[i].Color = InQuadColor.ToFColor(false);
        }

        for (int i = 3; i < topLeftTriangle.VertexCount(); ++i)
        {
            VertexIndices.Add(0);
            VertexIndices.Add(i - 1);
            VertexIndices.Add(i);
        }

        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            InOutLayerId,
            White->GetRenderingResource(),
            Vertices,
            VertexIndices,
            nullptr,
            0,
            0,
            ESlateDrawEffect::NoPixelSnapping
        );
    }

    if (topRightTriangle.Area() > 0.f)
    {
        TArray<FSlateVertex> Vertices;
        TArray<SlateIndex> VertexIndices = {0, 1, 2};
        Vertices.AddUninitialized(topRightTriangle.VertexCount());

        for (int i = 0; i < topRightTriangle.VertexCount(); ++i)
        {
            Vertices[i].Position = FVector2f(InAllottedGeometry.LocalToAbsolute(topRightTriangle[i]));
            Vertices[i].Color = InQuadColor.ToFColor(false);
        }

        for (int i = 3; i < topRightTriangle.VertexCount(); ++i)
        {
            VertexIndices.Add(0);
            VertexIndices.Add(i - 1);
            VertexIndices.Add(i);
        }

        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            InOutLayerId,
            White->GetRenderingResource(),
            Vertices,
            VertexIndices,
            nullptr,
            0,
            0,
            ESlateDrawEffect::NoPixelSnapping
        );
    }

    if (bottomRightTriangle.Area() > 0.f)
    {
        TArray<FSlateVertex> Vertices;
        TArray<SlateIndex> VertexIndices = {0, 1, 2};
        Vertices.AddUninitialized(bottomRightTriangle.VertexCount());

        for (int i = 0; i < bottomRightTriangle.VertexCount(); ++i)
        {
            Vertices[i].Position = FVector2f(InAllottedGeometry.LocalToAbsolute(bottomRightTriangle[i]));
            Vertices[i].Color = InQuadColor.ToFColor(false);
        }

        for (int i = 3; i < bottomRightTriangle.VertexCount(); ++i)
        {
            VertexIndices.Add(0);
            VertexIndices.Add(i - 1);
            VertexIndices.Add(i);
        }

        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            InOutLayerId,
            White->GetRenderingResource(),
            Vertices,
            VertexIndices,
            nullptr,
            0,
            0,
            ESlateDrawEffect::NoPixelSnapping
        );

    }

    if (bottomLeftTriangle.Area() > 0.f)
    {
        TArray<FSlateVertex> Vertices;
        TArray<SlateIndex> VertexIndices = {0, 1, 2};
        Vertices.AddUninitialized(bottomLeftTriangle.VertexCount());

        for (int i = 0; i < bottomLeftTriangle.VertexCount(); ++i)
        {
            Vertices[i].Position = FVector2f(InAllottedGeometry.LocalToAbsolute(bottomLeftTriangle[i]));
            Vertices[i].Color = InQuadColor.ToFColor(false);
        }

        for (int i = 3; i < bottomLeftTriangle.VertexCount(); ++i)
        {
            VertexIndices.Add(0);
            VertexIndices.Add(i - 1);
            VertexIndices.Add(i);
        }

        FSlateDrawElement::MakeCustomVerts(
            OutDrawElements,
            InOutLayerId,
            White->GetRenderingResource(),
            Vertices,
            VertexIndices,
            nullptr,
            0,
            0,
            ESlateDrawEffect::NoPixelSnapping
        );

    }

}
