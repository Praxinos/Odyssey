// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyHUDEllipse.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDEllipse::~FOdysseyHUDEllipse()
{

}

FOdysseyHUDEllipse::FOdysseyHUDEllipse(const FVector2D& iCenter, int iXRadius, int iYRadius)
    : mCenter(iCenter)
    , mXRadius(iXRadius)
    , mYRadius(iYRadius)
{
}

void
FOdysseyHUDEllipse::DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);

    ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateEllipsePoints( ::ULIS::FVec2I(mCenter.X, mCenter.Y), mXRadius, mYRadius, points );

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
FOdysseyHUDEllipse::SetCenter(const FVector2D& iCenter)
{
    mCenter = iCenter;
}

void
FOdysseyHUDEllipse::SetXRadius(int iRadius)
{
    mXRadius = iRadius;
}

void
FOdysseyHUDEllipse::SetYRadius(int iRadius)
{
    mYRadius = iRadius;
}

const FVector2D&
FOdysseyHUDEllipse::GetCenter() const
{
    return mCenter;
}

int
FOdysseyHUDEllipse::GetXRadius() const
{
    return mXRadius;
}

int
FOdysseyHUDEllipse::GetYRadius() const
{
    return mYRadius;
}
