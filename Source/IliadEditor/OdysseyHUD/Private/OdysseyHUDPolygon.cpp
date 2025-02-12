// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyHUDPolygon.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDPolygon::~FOdysseyHUDPolygon()
{
    mPoints.Empty();
}

FOdysseyHUDPolygon::FOdysseyHUDPolygon()
{
}

void
FOdysseyHUDPolygon::DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams)
{
    if (mPoints.Num() < 2)
        return;

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);
    const FLinearColor color(0.f, 1.f, 0.f);

    for (int i = 1; i < mPoints.Num(); i++)
    {
        FVector2D startPosition = iParams.mTextureToHUD.Execute(mPoints[i - 1]);
        FVector2D finishPosition = iParams.mTextureToHUD.Execute(mPoints[i]);

        batchedElements->AddTranslucentLine(
            FVector(startPosition, 0.f),
            FVector(finishPosition, 0.f),
            color,
            iParams.mCanvas->GetHitProxyId(),
            1.f,
            0.f,
            true
        );
    }

    //Close the polygon
    if (mClosePolygon)
    {
        FVector2D startPosition = iParams.mTextureToHUD.Execute(mPoints.Last());
        FVector2D finishPosition = iParams.mTextureToHUD.Execute(mPoints[0]);

        batchedElements->AddTranslucentLine(
            FVector(startPosition, 0.f),
            FVector(finishPosition, 0.f),
            color,
            iParams.mCanvas->GetHitProxyId(),
            1.f,
            0.f,
            true
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
