// IDDN.FR.001.060015.014.S.X.2019.000.00000
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

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

    TArray<FLinearColor> colors = customization.mColors;
    if( colors.Num() == 0 )
        colors.Add(FLinearColor::Black);

    int colorIndex = 0;
    int numColors = colors.Num();
    float cumulLength = 0.f;

    // Total pattern length (Segment + Gap)
    float patternLength = customization.mSegmentLength + customization.mGapLength;

    double time = FApp::GetCurrentTime();
    float timeOffset = FMath::Fmod(time * customization.mSpeed, patternLength);

    // Draw all polygon edges
    for (int i = 1; i < mPoints.Num(); i++)
    {
        DrawCustomizedLine( iParams.mCanvas,
                            iParams.mTextureToHUD.Execute(mPoints[i - 1]),
                            iParams.mTextureToHUD.Execute(mPoints[i]),
                            timeOffset,
                            patternLength,
                            cumulLength,
                            colorIndex,
                            colors,
                            customization,
                            batchedElements
                         );
    }

    // Close polygon if needed (continue pattern seamlessly)
    if (mClosePolygon)
    {
        DrawCustomizedLine(iParams.mCanvas,
            iParams.mTextureToHUD.Execute(mPoints.Last()),
            iParams.mTextureToHUD.Execute(mPoints[0]),
            timeOffset,
            patternLength,
            cumulLength,
            colorIndex,
            colors,
            customization,
            batchedElements
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
