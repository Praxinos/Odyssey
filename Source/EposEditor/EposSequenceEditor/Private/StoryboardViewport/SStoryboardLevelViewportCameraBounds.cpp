// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "StoryboardViewport/SStoryboardLevelViewportCameraBounds.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "StoryboardViewport/SStoryboardLevelViewport.h"
#include "StoryboardViewport/StoryboardVisibleArea.h"
#include "BoxTypes.h"
#include "Polygon2.h"
#include "Rendering/SlateRenderer.h"
#include "Fonts/FontMeasure.h"
#include "EngineFontServices.h"

#define LOCTEXT_NAMESPACE "StoryboardLevelViewportCameraBounds"

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
    if (settings && settings->ViewportSettings.bDisplayCameraBounds)
    {
        DrawCameraBounds(InPaintArgs, InAllottedGeometry, InMyCullingRect, OutDrawElements, InLayerId, settings->ViewportSettings.CameraBoundsShadeColor);
    }

    return InLayerId;
}

void SStoryboardLevelViewportCameraBounds::DrawCameraBounds(const FPaintArgs& InPaintArgs, const FGeometry& InAllottedGeometry,
    const FSlateRect& InMyCullingRect, FSlateWindowElementList& OutDrawElements, int32& InOutLayerId, const FLinearColor& InQuadColor) const
{
    UEposSequenceEditorSettings* settings = GetMutableDefault<UEposSequenceEditorSettings>();
    check( settings );

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

    bool bIsPilotingCamera = StoryboardLevelViewportClient->IsAnyActorLocked();
    //bool bIsViewCamera = StoryboardLevelViewportClient->IsPerspectiveViewportCameraCutEnabled();

    //FLinearColor quadColor = InQuadColor;
    //if( bIsPilotingCamera )
    //    quadColor = FLinearColor::Red.CopyWithNewOpacity( InQuadColor.A );
    //if( bIsViewCamera )
    //    quadColor = FLinearColor::Blue.CopyWithNewOpacity( InQuadColor.A );

    ++InOutLayerId;

    //---

    const FStoryboardVisibleArea& VisibleArea = StoryboardLevelViewportClient->GetZoomedVisibleArea();

    static const FSlateBrush* WhiteBrush = FAppStyle::Get().GetBrush("Brushes.White");

    const FVector2f CachedViewportSize( StoryboardLevelViewportClient->GetViewportGeometry().WidgetSize );
    if (FMath::IsNearlyZero(CachedViewportSize.X) || FMath::IsNearlyZero(CachedViewportSize.Y))
    {
        return;
    }

    FVector2f topLeft( VisibleArea.TopLeft );
    FVector2f topRight( VisibleArea.TopRight );
    FVector2f bottomLeft( VisibleArea.BottomLeft );
    FVector2f bottomRight( VisibleArea.BottomRight );

    FVector2f center( ( topLeft + bottomRight ) / 2.f );
    FVector2f up( topLeft - bottomLeft );
    up.Normalize();
    FVector2f down = -up;
    FVector2f right( topRight - topLeft );
    right.Normalize();
    FVector2f left = -right;

    // USE for DEBUGGING
    //FSlateDrawElement::MakeLines(
    //    OutDrawElements,
    //    InOutLayerId++,
    //    InAllottedGeometry.ToPaintGeometry(),
    //    {
    //        topLeft,
    //        topRight,
    //        bottomRight,
    //        bottomLeft,
    //        topLeft
    //    },
    //    ESlateDrawEffect::NoPixelSnapping,
    //    FLinearColor::Red,
    //    true,
    //    2.f
    //);

    ::UE::Geometry::FPolygon2f polygon(
        {
            topLeft,
            topRight,
            bottomRight,
            bottomLeft,
        }
    );

    ::UE::Geometry::FAxisAlignedBox2f bounds = polygon.Bounds();

    FVector2f topBoxPosition(0.f, 0.f);
    FVector2f topBoxSize(CachedViewportSize.X, FMath::Min(bounds.Min.Y, CachedViewportSize.Y));
    FVector2f bottomBoxPosition(0.f, bounds.Max.Y);
    FVector2f bottomBoxSize(CachedViewportSize.X, FMath::Max(CachedViewportSize.Y - bounds.Max.Y, 0.f));
    FVector2f leftBoxPosition(0.f, topBoxSize.Y);
    FVector2f leftBoxSize(FMath::Min(bounds.Min.X, CachedViewportSize.X), CachedViewportSize.Y - topBoxSize.Y - bottomBoxSize.Y);
    FVector2f rightBoxPosition(bounds.Max.X, topBoxSize.Y);
    FVector2f rightBoxSize(FMath::Max(CachedViewportSize.X - bounds.Max.X, 0.f), CachedViewportSize.Y - topBoxSize.Y - bottomBoxSize.Y);

    FVector2f topmostPoint = polygon[0];
    FVector2f bottommostPoint = polygon[0];
    FVector2f leftmostPoint = polygon[0];
    FVector2f rightmostPoint = polygon[0];
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

    ::UE::Geometry::FPolygon2f topLeftTriangle(
        {
            FVector2f(leftBoxSize.X, topBoxSize.Y),
            topmostPoint,
            leftmostPoint,
        }
    );

    ::UE::Geometry::FPolygon2f topRightTriangle(
        {
            FVector2f(rightBoxPosition.X, topBoxSize.Y),
            rightmostPoint,
            topmostPoint,
        }
    );

    ::UE::Geometry::FPolygon2f bottomRightTriangle(
        {
            FVector2f(rightBoxPosition.X, bottomBoxPosition.Y),
            bottommostPoint,
            rightmostPoint,
        }
    );

    ::UE::Geometry::FPolygon2f bottomLeftTriangle(
        {
            FVector2f(leftBoxSize.X, bottomBoxPosition.Y),
            leftmostPoint,
            bottommostPoint,
        }
    );

    ::UE::Geometry::FAxisAlignedBox2f viewportBox(FVector2f(0.f, 0.f), CachedViewportSize);

    topLeftTriangle.ClipConvex(viewportBox);
    topRightTriangle.ClipConvex(viewportBox);
    bottomRightTriangle.ClipConvex(viewportBox);
    bottomLeftTriangle.ClipConvex(viewportBox);

    //---

    if (bounds.Min.Y >= 0.f)
    {
        FSlateDrawElement::MakeBox(
            OutDrawElements,
            InOutLayerId,
            InAllottedGeometry.ToPaintGeometry(topBoxSize, FSlateLayoutTransform(topBoxPosition)),
            WhiteBrush,
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
            WhiteBrush,
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
            WhiteBrush,
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
            WhiteBrush,
            ESlateDrawEffect::NoPixelSnapping,
            InQuadColor
        );
    }

    //---

    if (topLeftTriangle.Area() > 0.f)
    {
        TArray<FSlateVertex> Vertices;
        TArray<SlateIndex> VertexIndices = {0, 1, 2};
        Vertices.AddUninitialized(topLeftTriangle.VertexCount());

        for (int i = 0; i < topLeftTriangle.VertexCount(); ++i)
        {
            Vertices[i].Position = FVector2f(InAllottedGeometry.LocalToAbsolute(topLeftTriangle[i]));
            Vertices[i].Color = InQuadColor.ToFColor(true);
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
            WhiteBrush->GetRenderingResource(),
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
            Vertices[i].Color = InQuadColor.ToFColor(true);
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
            WhiteBrush->GetRenderingResource(),
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
            Vertices[i].Color = InQuadColor.ToFColor(true);
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
            WhiteBrush->GetRenderingResource(),
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
            Vertices[i].Color = InQuadColor.ToFColor(true);
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
            WhiteBrush->GetRenderingResource(),
            Vertices,
            VertexIndices,
            nullptr,
            0,
            0,
            ESlateDrawEffect::NoPixelSnapping
        );
    }

    //---

    InOutLayerId++;

    if( bIsPilotingCamera && settings->ViewportSettings.bDisplayPilotingCameraHUD )
    {
        //--- Draw the upper and lower semi-circles

        auto DrawArc = [&]( FVector2f iP0, FVector2f iP1, FVector2f iP2, FVector2f iP3, float iRadius, float iThickness, FLinearColor iColor )
            {
                FSlateDrawElement::MakeCubicBezierSpline(
                    OutDrawElements,
                    InOutLayerId,
                    //InAllottedGeometry.ToPaintGeometry(),
                    InAllottedGeometry.ToPaintGeometry( FSlateLayoutTransform( center ) ),
                    //InAllottedGeometry.ToPaintGeometry( FSlateLayoutTransform( iRadius, center ) ),
                    //InAllottedGeometry.ToPaintGeometry( FVector2f( 100, 100 ), FSlateLayoutTransform( center ) ),
                    iP0 * iRadius,
                    iP1 * iRadius,
                    iP2 * iRadius,
                    iP3 * iRadius,
                    iThickness,
                    ESlateDrawEffect::NoPixelSnapping,
                    iColor
                );
            };

        float thickness = 2.f;
        float radius = ( topLeft - center ).Length();
        FLinearColor color = ( FLinearColor::White - InQuadColor ).CopyWithNewOpacity( InQuadColor.A );
        float offset = 1.4f;
        float offset2 = 1.8f;

        //https://spencermortensen.com/articles/bezier-circle/
        auto RotInPlace = []( FVector2f& ioVector ) -> FVector2f&
            {
                ioVector = -FVector2f( ioVector.Y, -ioVector.X );
                return ioVector;
            };

        //https://mechanicalexpressions.com/explore/geometric-modeling/circle-spline.html
        float strength = ( -4 + 4 * FMath::Sqrt( 2.f ) ) / 3.f;
        FVector2f p0 = up;
        FVector2f p1 = up + right * strength;
        FVector2f p2 = right + up * strength;
        FVector2f p3 = right;

        DrawArc( p0, p1, p2, p3, radius, thickness, color );
        DrawArc( RotInPlace( p0 ), RotInPlace( p1 ), RotInPlace( p2 ), RotInPlace( p3 ), radius * offset, thickness, color );
        DrawArc( RotInPlace( p0 ), RotInPlace( p1 ), RotInPlace( p2 ), RotInPlace( p3 ), radius * offset, thickness, color );
        DrawArc( RotInPlace( p0 ), RotInPlace( p1 ), RotInPlace( p2 ), RotInPlace( p3 ), radius, thickness, color );

        //--- Draw the left and right "horizon" dashed lines

        FSlateDrawElement::MakeDashedLines(
            OutDrawElements
            , InOutLayerId
            , InAllottedGeometry.ToPaintGeometry( FSlateLayoutTransform( center ) )
            , { right * radius, right * radius * offset }
            , ESlateDrawEffect::NoPixelSnapping
            , color
            , thickness
            //, 10.f //DashLengthPx
            //, 0.f  //DashScreenOffset
        );

        FSlateDrawElement::MakeDashedLines(
            OutDrawElements
            , InOutLayerId
            , InAllottedGeometry.ToPaintGeometry( FSlateLayoutTransform( center ) )
            , { left * radius, left * radius * offset }
            , ESlateDrawEffect::NoPixelSnapping
            , color
            , thickness
            //, 10.f //DashLengthPx
            //, 0.f  //DashScreenOffset
        );

        //--- Draw the left and right "piloting" texts above the dashed lines

        FText text = LOCTEXT( "storyboard-viewport.camera-bounds.piloting.label", "Piloting" );
        FSlateFontInfo fontInfo = FCoreStyle::GetDefaultFontStyle( "Regular", 25 * StoryboardLevelViewportClient->GetZoomController().GetZoom() );
        TSharedRef<FSlateFontMeasure> fontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

        FVector2f textSize = fontMeasureService->Measure( text, fontInfo );

        FVector2f middleLine = ( left * radius + left * radius * offset ) / 2.f;
        FVector2f startTopLeftText = middleLine + left * textSize.X / 2.f + up * textSize.Y;

        TSharedPtr<FSlateFontCache> fontCache = FEngineFontServices::Get().GetFontCache();
        FShapedGlyphSequenceRef glyphSequence = fontCache->ShapeBidirectionalText( text.ToString(), fontInfo, 1.f, TextBiDi::ETextDirection::LeftToRight, ETextShapingMethod::Auto );

        FSlateDrawElement::MakeRotatedShapedText(
            OutDrawElements
            , InOutLayerId
            , InAllottedGeometry.ToPaintGeometry( FSlateLayoutTransform( center + startTopLeftText ) )
            , glyphSequence
            , ESlateDrawEffect::NoPixelSnapping
            , color
            , color
            , FMath::DegreesToRadians( StoryboardLevelViewportClient->GetZoomController().GetRotation() )
            , FVector2f( 0, 0 ) // InRotationPoint (Must be set to 0,0 and not use the default optional value)
            , FSlateDrawElement::ERotationSpace::RelativeToElement
        );

        middleLine = ( right * radius + right * radius * offset ) / 2.f;
        startTopLeftText = middleLine + left * textSize.X / 2.f + up * textSize.Y;

        FSlateDrawElement::MakeRotatedShapedText(
            OutDrawElements
            , InOutLayerId
            , InAllottedGeometry.ToPaintGeometry( FSlateLayoutTransform( center + startTopLeftText ) )
            , glyphSequence
            , ESlateDrawEffect::NoPixelSnapping
            , color
            , color
            , FMath::DegreesToRadians( StoryboardLevelViewportClient->GetZoomController().GetRotation() )
            , FVector2f( 0, 0 ) // InRotationPoint (Must be set to 0,0 and not use the default optional value)
            , FSlateDrawElement::ERotationSpace::RelativeToElement
        );
    }
}

#undef LOCTEXT_NAMESPACE
