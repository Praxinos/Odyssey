// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDPolygon.h"

#include "ULISLoaderModule.h"

FOdysseyHUDPolygon::~FOdysseyHUDPolygon()
{
    mPoints.Empty();
}

FOdysseyHUDPolygon::FOdysseyHUDPolygon(FName iName) :
    FOdysseyHUDElement(iName)
{
}

void
FOdysseyHUDPolygon::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    if (mPoints.Num() < 2)
        return;
    
    const FLinearColor color(0.f, 1.f, 0.f);

    for (int i = 1; i < mPoints.Num(); i++)
    {
        FVector startPosition = iParams.mOrigin
            + mPoints[i - 1].X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
            + mPoints[i - 1].Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

        FVector finishPosition = iParams.mOrigin
            + mPoints[i].X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
            + mPoints[i].Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

        iParams.mPDI->DrawTranslucentLine(
            startPosition,
            finishPosition,
            color,
            SDPG_Foreground,
            1.0f,
            0.0f,
            true
        );
    }

    //Close the polygon
    FVector startPosition = iParams.mOrigin
        + mPoints.Last().X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mPoints.Last().Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    FVector finishPosition = iParams.mOrigin
        + mPoints[0].X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mPoints[0].Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

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

TArray<FVector2D>&
FOdysseyHUDPolygon::GetPoints()
{
    return mPoints;
}
