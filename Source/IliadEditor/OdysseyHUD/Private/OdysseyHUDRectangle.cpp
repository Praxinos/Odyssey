// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDRectangle.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDRectangle::~FOdysseyHUDRectangle()
{

}

FOdysseyHUDRectangle::FOdysseyHUDRectangle(const FVector2D& iTopLeftPoint, const FVector2D& iBottomRightPoint)
    : mTopLeftPoint(iTopLeftPoint)
    , mBottomRightPoint(iBottomRightPoint)
{
}

void
FOdysseyHUDRectangle::DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);
    FVector2D topLeft = iParams.mTextureToHUD.Execute(mTopLeftPoint);
    FVector2D topRight = iParams.mTextureToHUD.Execute(FVector2D(mBottomRightPoint.X, mTopLeftPoint.Y));
    FVector2D bottomRight = iParams.mTextureToHUD.Execute(mBottomRightPoint);
    FVector2D bottomLeft = iParams.mTextureToHUD.Execute(FVector2D(mTopLeftPoint.X, mBottomRightPoint.Y));

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);
    batchedElements->AddTranslucentLine(FVector(topLeft, 0.f), FVector(topRight, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    batchedElements->AddTranslucentLine(FVector(topRight, 0.f), FVector(bottomRight, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    batchedElements->AddTranslucentLine(FVector(bottomRight, 0.f), FVector(bottomLeft, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    batchedElements->AddTranslucentLine(FVector(bottomLeft, 0.f), FVector(topLeft, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDRectangle::SetTopLeftPoint(const FVector2D& iPoint)
{
    mTopLeftPoint = iPoint;
}

void
FOdysseyHUDRectangle::SetBottomRightPoint(const FVector2D& iPoint)
{
    mBottomRightPoint = iPoint;
}

const FVector2D&
FOdysseyHUDRectangle::GetTopLeftPoint() const
{
    return mTopLeftPoint;
}

const FVector2D&
FOdysseyHUDRectangle::GetBottomRightPoint() const
{
    return mBottomRightPoint;
}
