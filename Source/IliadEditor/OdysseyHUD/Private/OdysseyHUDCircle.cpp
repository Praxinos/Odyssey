// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDCircle.h"

#include "BatchedElements.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDCircle::~FOdysseyHUDCircle()
{

}

FOdysseyHUDCircle::FOdysseyHUDCircle(const FVector2D& iCenterPoint, float iRadius)
{
    mCenterPoint = iCenterPoint;
    mRadius = iRadius;
}

void
FOdysseyHUDCircle::DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);

    ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateCirclePoints( ::ULIS::FVec2I(mCenterPoint.X, mCenterPoint.Y), mRadius, points );

    if (points.Size() < 2)
        return;

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

    for (int i = 1; i < points.Size(); i++)
    {
        FVector2D startPoint = iParams.mTextureToHUD.Execute(FVector2D(points[i - 1].x, points[i - 1].y));
        FVector2D endPoint = iParams.mTextureToHUD.Execute(FVector2D(points[i].x, points[i].y));

        batchedElements->AddTranslucentLine(FVector(startPoint, 0.f), FVector(endPoint, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    }

    FVector2D startPoint = iParams.mTextureToHUD.Execute(FVector2D(points[points.Size() - 1].x, points[points.Size() - 1].y));
    FVector2D endPoint = iParams.mTextureToHUD.Execute(FVector2D(points[0].x, points[0].y));
    batchedElements->AddTranslucentLine(FVector(startPoint, 0.f), FVector(endPoint, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDCircle::SetCenter(const FVector2D& iCenterPoint)
{
    mCenterPoint = iCenterPoint;
}

void
FOdysseyHUDCircle::SetRadius(float iRadius)
{
    mRadius = iRadius;
}
