// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDLine.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDLine::~FOdysseyHUDLine()
{

}

FOdysseyHUDLine::FOdysseyHUDLine(const FVector2D& iStartPoint, const FVector2D& iEndPoint)
    : mStartPoint(iStartPoint)
    , mEndPoint(iEndPoint)
{

}

void
FOdysseyHUDLine::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    FLinearColor RenderColor = mColor;
    RenderColor.A *= mOpacity;
    InitDrawCustomizedLine(iParams.mCanvas, customization, RenderColor, mThickness);

    DrawCustomizedLine(
        iParams,
        mStartPoint,
        mEndPoint
    );

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDLine::SetStartPoint(const FVector2D& iPoint)
{
    mStartPoint = iPoint;
}

void
FOdysseyHUDLine::SetEndPoint(const FVector2D& iPoint)
{
    mEndPoint = iPoint;
}

const FVector2D&
FOdysseyHUDLine::GetStartPoint() const
{
    return mStartPoint;
}

const FVector2D&
FOdysseyHUDLine::GetEndPoint() const
{
    return mEndPoint;
}

void FOdysseyHUDLine::SetColor(const FLinearColor& iColor)
{
    mColor = iColor;
}

const FLinearColor& FOdysseyHUDLine::GetColor() const
{
    return mColor;
}

void FOdysseyHUDLine::SetOpacity(float iOpacity)
{
    mOpacity = FMath::Clamp(iOpacity, 0.f, 1.f);
}

float FOdysseyHUDLine::GetOpacity() const
{
    return mOpacity;
}

void FOdysseyHUDLine::SetThickness(float iThickness)
{
    mThickness = FMath::Max(0.f, iThickness);
}

float FOdysseyHUDLine::GetThickness() const
{
    return mThickness;
}
