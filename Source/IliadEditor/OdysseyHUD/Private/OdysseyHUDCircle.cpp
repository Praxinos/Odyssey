// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDCircle.h"

#include "BatchedElements.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDCircle::~FOdysseyHUDCircle()
{

}

FOdysseyHUDCircle::FOdysseyHUDCircle(const FVector2D& iCenterPoint, float iRadius)
{
    mCenterPoint = iCenterPoint;
    mRadius = iRadius;
}

TArray<FVector2D> GenerateCirclePoints(const FVector2D& InCenter, float InRadius, uint32 InNumSides)
{
    TArray<FVector2D> Points;

    if (InNumSides <= 2)
        return Points;

    const float AngleDelta = 2.0f * UE_PI / InNumSides;
    FVector2D LastPoint = InCenter + FVector2D(InRadius, 0);

    for(uint32 SideIndex = 0; SideIndex < InNumSides; SideIndex++)
    {
        const FVector2D Point = InCenter + FVector2D(FMath::Cos(AngleDelta * (SideIndex + 1)) * InRadius, FMath::Sin(AngleDelta * (SideIndex + 1))* InRadius);
        Points.Add(Point);
        LastPoint = Point;
    }

    return Points;
}

void
FOdysseyHUDCircle::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    /* ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateCirclePoints(::ULIS::FVec2I(0, 0), mRadius, points); */

    TArray<FVector2D> points = GenerateCirclePoints(FVector2D(0, 0), mRadius, 50);
    InitDrawCustomizedLine(iParams.mCanvas, customization, FLinearColor::Black);

    for (int i = 1; i < points.Num(); i++)
    {
        FVector2D startPoint = FVector2D(points[i - 1].X, points[i - 1].Y) + mCenterPoint;
        FVector2D endPoint = FVector2D(points[i].X, points[i].Y) + mCenterPoint;

        DrawCustomizedLine(iParams, startPoint, endPoint);
    }

    FVector2D startPoint = FVector2D(points[points.Num() - 1].X, points[points.Num() - 1].Y) + mCenterPoint;
    FVector2D endPoint = FVector2D(points[0].X, points[0].Y) + mCenterPoint;

    DrawCustomizedLine(iParams, startPoint, endPoint);

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDCircle::SetCenter(const FVector2D& iCenterPoint)
{
    mCenterPoint = iCenterPoint;
}

void
FOdysseyHUDCircle::SetRadius(float iRadius)
{
    mRadius = iRadius;
}

const FVector2D&
FOdysseyHUDCircle::GetCenter() const
{
    return mCenterPoint;
}

float
FOdysseyHUDCircle::GetRadius() const
{
    return mRadius;
}
