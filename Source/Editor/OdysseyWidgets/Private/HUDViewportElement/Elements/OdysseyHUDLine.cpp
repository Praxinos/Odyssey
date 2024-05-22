// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDLine.h"

#include "ULISLoaderModule.h"

FOdysseyHUDLine::~FOdysseyHUDLine()
{

}

FOdysseyHUDLine::FOdysseyHUDLine(FName iName, FVector2D iStartPoint, FVector2D iFinishPoint) :
    FOdysseyHUDElement(iName)
{
    mStartPoint = iStartPoint;
    mFinishPoint = iFinishPoint;
}

void
FOdysseyHUDLine::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);
    FVector startPosition = iParams.mOrigin
        + mStartPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mStartPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector finishPosition = iParams.mOrigin
        + mFinishPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mFinishPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    iParams.mPDI->DrawTranslucentLine(
		startPosition,
		finishPosition,
		color,
		SDPG_Foreground,
		1.0f,
		0.0f,
		true
	);

    FOdysseyHUDElement::Render(iParams);
}
