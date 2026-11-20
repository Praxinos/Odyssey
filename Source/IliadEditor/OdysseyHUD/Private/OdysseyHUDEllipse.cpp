// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDEllipse.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDEllipse::~FOdysseyHUDEllipse()
{

}

FOdysseyHUDEllipse::FOdysseyHUDEllipse(const FVector2D& iCenter, int iXRadius, int iYRadius)
    : mCenter(iCenter)
    , mXRadius(iXRadius)
    , mYRadius(iYRadius)
{
}

void
FOdysseyHUDEllipse::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateEllipsePoints(::ULIS::FVec2I(mCenter.X, mCenter.Y), mXRadius, mYRadius, points);

    if (points.Size() < 2)
        return;

    FLinearColor RenderColor = mColor;
    RenderColor.A *= mOpacity;
    InitDrawCustomizedLine(iParams.mCanvas, customization, RenderColor, mThickness);

    for (int i = 1; i < points.Size(); i++)
    {
        const FVector2D StartOffset = FVector2D(points[i - 1].x, points[i - 1].y) - mCenter;
        const FVector2D EndOffset = FVector2D(points[i].x, points[i].y) - mCenter;
        const FVector2D StartPoint = mCenter + StartOffset.GetRotated(mRotationDegrees);
        const FVector2D EndPoint = mCenter + EndOffset.GetRotated(mRotationDegrees);

        DrawCustomizedLine(
            iParams,
            StartPoint,
            EndPoint
        );
    }

    const FVector2D StartOffset = FVector2D(points[points.Size() - 1].x, points[points.Size() - 1].y) - mCenter;
    const FVector2D EndOffset = FVector2D(points[0].x, points[0].y) - mCenter;
    const FVector2D StartPoint = mCenter + StartOffset.GetRotated(mRotationDegrees);
    const FVector2D EndPoint = mCenter + EndOffset.GetRotated(mRotationDegrees);

    DrawCustomizedLine(
        iParams,
        StartPoint,
        EndPoint
    );

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDEllipse::SetCenter(const FVector2D& iCenter)
{
    mCenter = iCenter;
}

void
FOdysseyHUDEllipse::SetXRadius(int iRadius)
{
    mXRadius = iRadius;
}

void
FOdysseyHUDEllipse::SetYRadius(int iRadius)
{
    mYRadius = iRadius;
}

const FVector2D&
FOdysseyHUDEllipse::GetCenter() const
{
    return mCenter;
}

int
FOdysseyHUDEllipse::GetXRadius() const
{
    return mXRadius;
}

int
FOdysseyHUDEllipse::GetYRadius() const
{
    return mYRadius;
}

void FOdysseyHUDEllipse::SetRotation(float iRotationDegrees)
{
    mRotationDegrees = iRotationDegrees;
}

float FOdysseyHUDEllipse::GetRotation() const
{
    return mRotationDegrees;
}

void FOdysseyHUDEllipse::SetColor(const FLinearColor& iColor)
{
    mColor = iColor;
}

const FLinearColor& FOdysseyHUDEllipse::GetColor() const
{
    return mColor;
}

void FOdysseyHUDEllipse::SetOpacity(float iOpacity)
{
    mOpacity = FMath::Clamp(iOpacity, 0.f, 1.f);
}

float FOdysseyHUDEllipse::GetOpacity() const
{
    return mOpacity;
}

void FOdysseyHUDEllipse::SetThickness(float iThickness)
{
    mThickness = FMath::Max(0.f, iThickness);
}

float FOdysseyHUDEllipse::GetThickness() const
{
    return mThickness;
}
