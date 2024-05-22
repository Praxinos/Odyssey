// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDRectangle.h"

#include "ULISLoaderModule.h"

FOdysseyHUDRectangle::~FOdysseyHUDRectangle()
{

}

FOdysseyHUDRectangle::FOdysseyHUDRectangle(FName iName, FVector2D iTopLeftPoint, FVector2D iBottomRightPoint) :
    FOdysseyHUDElement(iName)
{
    mTopLeftPoint = iTopLeftPoint;
    mBottomRightPoint = iBottomRightPoint;
}

void
FOdysseyHUDRectangle::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);
    FVector topLeft = iParams.mOrigin
        + mTopLeftPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mTopLeftPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector topRight = iParams.mOrigin
        + mBottomRightPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mTopLeftPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector bottomRight = iParams.mOrigin
        + mBottomRightPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mBottomRightPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector bottomLeft = iParams.mOrigin
        + mTopLeftPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mBottomRightPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    iParams.mPDI->DrawTranslucentLine(
        topLeft,
        topRight,
        color,
        SDPG_Foreground,
        1.0f,
        0.0f,
        true
    );

    iParams.mPDI->DrawTranslucentLine(
        topRight,
        bottomRight,
        color,
        SDPG_Foreground,
        1.0f,
        0.0f,
        true
    );

    iParams.mPDI->DrawTranslucentLine(
        bottomRight,
        bottomLeft,
        color,
        SDPG_Foreground,
        1.0f,
        0.0f,
        true
    );

    iParams.mPDI->DrawTranslucentLine(
        bottomLeft,
        topLeft,
        color,
        SDPG_Foreground,
        1.0f,
        0.0f,
        true
    );

    FOdysseyHUDElement::Render(iParams);
}
