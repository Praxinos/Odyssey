// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDCircle.h"

#include "ULISLoaderModule.h"

FOdysseyHUDCircle::~FOdysseyHUDCircle()
{

}

FOdysseyHUDCircle::FOdysseyHUDCircle(FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint) :
    FOdysseyHUDElement(iName)
{
    mCenterPoint = iCenterPoint;
    mBorderPoint = iBorderPoint;
    mRadius = (int)::ULIS::FMath::Dist(mCenterPoint.X, mCenterPoint.Y, mBorderPoint.X, mBorderPoint.Y);
}

FOdysseyHUDCircle::FOdysseyHUDCircle(FName iName, FVector2D iCenterPoint, float iRadius) :
    FOdysseyHUDElement(iName)
{
    mCenterPoint = iCenterPoint;
    mRadius = iRadius;
    mBorderPoint = FVector2D(mCenterPoint.X + iRadius, mCenterPoint.Y);
}

void
FOdysseyHUDCircle::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);
    
    FVector center = iParams.mOrigin
        + mCenterPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mCenterPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    float radius = mRadius / iParams.mTextureWidth * iParams.mPlaneWidth;

    DrawCircle(
        iParams.mPDI,
        center,
        iParams.mXAxis,
        iParams.mYAxis,
        color,
        radius,
        64,
        SDPG_Foreground,
        1.f,
        0.f,
        true
    );

    FOdysseyHUDElement::Render(iParams);
}

void
FOdysseyHUDCircle::SetCenter(const FVector2D iCenterPoint)
{
    mCenterPoint = iCenterPoint;
    mBorderPoint = FVector2D(mCenterPoint.X + mRadius, mCenterPoint.Y);
}

void
FOdysseyHUDCircle::SetRadius(float iRadius)
{
    mRadius = iRadius;
    mBorderPoint = FVector2D(mCenterPoint.X + mRadius, mCenterPoint.Y);
}
