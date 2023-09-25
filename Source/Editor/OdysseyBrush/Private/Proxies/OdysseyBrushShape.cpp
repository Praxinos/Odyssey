// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Proxies/OdysseyBrushShape.h"
#include <ULIS>

#define LOCTEXT_NAMESPACE "OdysseyBrushShape"

//////////////////////////////////////////////////////////////////////////
// UOdysseyBrushShape
//--------------------------------------------------------------------------------------
//--------------------------------------------- Odyssey Brush Blueprint Callable Methods

//static
void
UOdysseyBrushShape::GenerateLinePoints( FVector2D StartPoint, FVector2D EndPoint, TArray<FVector2D>& GeneratedPoints )
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateLinePoints(::ULIS::FVec2I(StartPoint.X, StartPoint.Y), ::ULIS::FVec2I(EndPoint.X, EndPoint.Y), pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add( FVector2D(pointsArray[i].x, pointsArray[i].y) );
    }
}

//static
void 
UOdysseyBrushShape::GenerateArcPoints(FVector2D Center, int Radius, int StartDegree, int EndDegree, TArray<FVector2D>& GeneratedPoints)
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateArcPoints(::ULIS::FVec2I(Center.X, Center.Y), Radius, StartDegree, EndDegree, pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add(FVector2D(pointsArray[i].x, pointsArray[i].y));
    }
}

//static
void
UOdysseyBrushShape::GenerateCirclePoints(FVector2D Center, int Radius, TArray<FVector2D>& GeneratedPoints)
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateCirclePoints(::ULIS::FVec2I(Center.X, Center.Y), Radius, pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add(FVector2D(pointsArray[i].x, pointsArray[i].y));
    }
}

//static
void
UOdysseyBrushShape::GenerateEllipsePoints(FVector2D Center, int A, int B, int Rotation, TArray<FVector2D>& GeneratedPoints)
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateRotatedEllipsePoints(::ULIS::FVec2I(Center.X, Center.Y), A, B, Rotation, pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add(FVector2D(pointsArray[i].x, pointsArray[i].y));
    }
}

//static
void 
UOdysseyBrushShape::GenerateQuadraticBezierPoints(FVector2D Start, FVector2D ControlPoint, FVector2D End, float iWeight, TArray<FVector2D>& GeneratedPoints)
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateQuadraticBezierPoints(::ULIS::FVec2I(Start.X, Start.Y), ::ULIS::FVec2I(ControlPoint.X, ControlPoint.Y), ::ULIS::FVec2I(End.X, End.Y), iWeight, pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add(FVector2D(pointsArray[i].x, pointsArray[i].y));
    }
}

//static
void 
UOdysseyBrushShape::GenerateRectanglePoints(FVector2D TopLeft, FVector2D BottomRight, TArray<FVector2D>& GeneratedPoints)
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateRectanglePoints(::ULIS::FVec2I(TopLeft.X, TopLeft.Y), ::ULIS::FVec2I(BottomRight.X, BottomRight.Y), pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add(FVector2D(pointsArray[i].x, pointsArray[i].y));
    }
}

//static
void
UOdysseyBrushShape::GeneratePolygonPoints(TArray<FVector2D> PolygonPoints, TArray<FVector2D>& GeneratedPoints)
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    std::vector<::ULIS::FVec2I> polygonPoints;

    for( int i = 0; i < PolygonPoints.Num(); i++) 
        polygonPoints.push_back( ::ULIS::FVec2I( PolygonPoints[i].X, PolygonPoints[i].Y ));

    ::ULIS::GeneratePolygonPoints(polygonPoints, pointsArray);

    for (int i = 0; i < pointsArray.Size(); i++)
    {
        GeneratedPoints.Add(FVector2D(pointsArray[i].x, pointsArray[i].y));
    }
}

#undef LOCTEXT_NAMESPACE

