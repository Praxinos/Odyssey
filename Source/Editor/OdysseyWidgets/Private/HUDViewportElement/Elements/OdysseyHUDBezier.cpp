// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDBezier.h"

#include "ULISLoaderModule.h"

FOdysseyHUDBezier::~FOdysseyHUDBezier()
{

}

FOdysseyHUDBezier::FOdysseyHUDBezier(FName iName, FVector2D iStartPoint, FVector2D iEndPoint, FVector2D iControlPoint) :
    FOdysseyHUDElement(iName)
{
    mStartPoint = iStartPoint;
    mEndPoint = iEndPoint;
    mControlPoint = iControlPoint;
}

void
FOdysseyHUDBezier::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    const FLinearColor bezierLineColor(0.f, 1.f, 0.f, 1.f);
    const FLinearColor controlLineColor(0.f, 1.f, 0.f, 0.4f);
    
    FVector startPoint = iParams.mOrigin
        + mStartPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mStartPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector controlPoint = iParams.mOrigin
        + mControlPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mControlPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector endPoint = iParams.mOrigin
        + mEndPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mEndPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateQuadraticBezierPoints(
        ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y),
        ::ULIS::FVec2I(mControlPoint.X, mControlPoint.Y),
        ::ULIS::FVec2I(mEndPoint.X, mEndPoint.Y),
        1.0f,
        pointsArray
    );

    if (pointsArray.Size() < 2)
        return;
        
    for (int i = 1; i < pointsArray.Size(); i++)
    {
        FVector startBezierPoint = iParams.mOrigin
            + ((float)pointsArray[i - 1].x) / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
            + ((float)pointsArray[i - 1].y) / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

        FVector endBezierPoint = iParams.mOrigin
            + ((float)pointsArray[i].x) / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
            + ((float)pointsArray[i].y) / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

        iParams.mPDI->DrawTranslucentLine(
            startBezierPoint,
            endBezierPoint,
            bezierLineColor,
            SDPG_Foreground,
            1.0f,
            0.0f,
            true
        );
    }

    iParams.mPDI->DrawTranslucentLine(
		startPoint,
		controlPoint,
		controlLineColor,
		SDPG_Foreground,
		1.0f,
		0.0f,
		true
	);

    iParams.mPDI->DrawTranslucentLine(
		controlPoint,
        endPoint, 
		controlLineColor,
		SDPG_Foreground,
		1.0f,
		0.0f,
		true
	);

    FOdysseyHUDElement::Render(iParams);
}
