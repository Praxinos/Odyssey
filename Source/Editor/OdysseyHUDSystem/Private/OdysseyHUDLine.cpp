// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
FOdysseyHUDLine::DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);

    FVector2D startPoint = iParams.mTextureToHUD.Execute(mStartPoint);
    FVector2D endPoint = iParams.mTextureToHUD.Execute(mEndPoint);

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);
    batchedElements->AddTranslucentLine(FVector(startPoint, 0.f), FVector(endPoint, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);

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