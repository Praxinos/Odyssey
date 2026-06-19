// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDPolygon.h"
#include "CanvasTypes.h"

#include "OdysseyHUDUtils.h"
#include "ULISLoaderModule.h"
#include "Misc/App.h"

FOdysseyHUDPolygon::~FOdysseyHUDPolygon()
{
    mPoints.Empty();
}

FOdysseyHUDPolygon::FOdysseyHUDPolygon()
{
}

void
FOdysseyHUDPolygon::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    if (mPoints.Num() < 2)
        return;

    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;
    if( customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f )
        return;

    InitDrawCustomizedLine(iParams.mCanvas, customization, FLinearColor::Black);

    // Draw all polygon edges
    for (int i = 1; i < mPoints.Num(); i++)
    {
        DrawCustomizedLine(
            iParams,
            mPoints[i - 1],
            mPoints[i]
        );
    }

    // Close polygon if needed (continue pattern seamlessly)
    if (mClosePolygon)
    {
        DrawCustomizedLine(
            iParams,
            mPoints.Last(),
            mPoints[0]
        );
    }

    FOdysseyHUDElement::DrawHUD(iParams);
}


TArray<FVector2D>&
FOdysseyHUDPolygon::GetPoints()
{
    return mPoints;
}

void
FOdysseyHUDPolygon::ClosePolygon(bool iClosePolygon)
{
    mClosePolygon = iClosePolygon;
}
