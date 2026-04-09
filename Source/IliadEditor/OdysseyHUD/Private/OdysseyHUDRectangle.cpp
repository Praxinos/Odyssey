// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDRectangle.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDRectangle::~FOdysseyHUDRectangle()
{

}

FOdysseyHUDRectangle::FOdysseyHUDRectangle(const FVector2D& iTopLeftPoint, const FVector2D& iBottomRightPoint)
    : mTopLeftPoint(iTopLeftPoint)
    , mBottomRightPoint(iBottomRightPoint)
{
}

void
FOdysseyHUDRectangle::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    FVector2D topLeft = mTopLeftPoint;
    FVector2D topRight = FVector2D(mBottomRightPoint.X, mTopLeftPoint.Y);
    FVector2D bottomRight = mBottomRightPoint;
    FVector2D bottomLeft = FVector2D(mTopLeftPoint.X, mBottomRightPoint.Y);

    InitDrawCustomizedLine(iParams.mCanvas, customization, FLinearColor::Black);
    DrawCustomizedLine(
        iParams,
        topLeft,
        topRight
    );

    DrawCustomizedLine(
        iParams,
        topRight,
        bottomRight
    );

    DrawCustomizedLine(
        iParams,
        bottomRight,
        bottomLeft
    );

    DrawCustomizedLine(
        iParams,
        bottomLeft,
        topLeft
    );

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDRectangle::SetTopLeftPoint(const FVector2D& iPoint)
{
    mTopLeftPoint = iPoint;
}

void
FOdysseyHUDRectangle::SetBottomRightPoint(const FVector2D& iPoint)
{
    mBottomRightPoint = iPoint;
}

const FVector2D&
FOdysseyHUDRectangle::GetTopLeftPoint() const
{
    return mTopLeftPoint;
}

const FVector2D&
FOdysseyHUDRectangle::GetBottomRightPoint() const
{
    return mBottomRightPoint;
}
