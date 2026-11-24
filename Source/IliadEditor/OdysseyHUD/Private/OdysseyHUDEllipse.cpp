// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDEllipse.h"
#include "CanvasTypes.h"

FOdysseyHUDEllipse::~FOdysseyHUDEllipse()
{

}

FOdysseyHUDEllipse::FOdysseyHUDEllipse(const FVector2D& iCenter, const FVector2D& iRadius)
    : mCenter(iCenter)
    , mRadius(iRadius)
{
}

void
FOdysseyHUDEllipse::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    if (mRadius.X <= 0.0 && mRadius.Y <= 0.0)
        return;

    FLinearColor RenderColor = mColor;
    RenderColor.A *= mOpacity;
    InitDrawCustomizedLine(iParams.mCanvas, customization, RenderColor, mThickness);

    // Estimate the on-screen radius to pick a segment count: about one segment every 4 screen pixels.
    double screenRadius = FMath::Max(mRadius.X, mRadius.Y);
    if (GetReference() == EOdysseyHUDReference::Texture && iParams.mTextureToHUD.IsBound())
    {
        const FVector2D screenCenter = iParams.mTextureToHUD.Execute(mCenter);
        const FVector2D screenEdge = iParams.mTextureToHUD.Execute(mCenter + FVector2D(screenRadius, 0.0));
        screenRadius = FVector2D::Distance(screenCenter, screenEdge);
    }
    const int32 segmentCount = FMath::Clamp(FMath::CeilToInt32(UE_TWO_PI * screenRadius / 4.0), 16, 2048);

    const double rotationRadians = FMath::DegreesToRadians(mRotationDegrees);
    const FVector2D axisX(FMath::Cos(rotationRadians), FMath::Sin(rotationRadians));
    const FVector2D axisY(-axisX.Y, axisX.X);

    // Point at angle t on the ellipse: center + axisX * radiusX * cos(t) + axisY * radiusY * sin(t). At t = 0 it is center + axisX * radiusX.
    FVector2D previous = mCenter + axisX * mRadius.X;
    for (int32 i = 1; i <= segmentCount; i++)
    {
        const double t = UE_TWO_PI * i / segmentCount;
        const FVector2D current = mCenter + axisX * (mRadius.X * FMath::Cos(t)) + axisY * (mRadius.Y * FMath::Sin(t));
        DrawCustomizedLine(iParams, previous, current);
        previous = current;
    }

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDEllipse::SetCenter(const FVector2D& iCenter)
{
    mCenter = iCenter;
}

void
FOdysseyHUDEllipse::SetRadius(const FVector2D& iRadius)
{
    mRadius = iRadius;
}

const FVector2D&
FOdysseyHUDEllipse::GetRadius() const
{
    return mRadius;
}

const FVector2D&
FOdysseyHUDEllipse::GetCenter() const
{
    return mCenter;
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
