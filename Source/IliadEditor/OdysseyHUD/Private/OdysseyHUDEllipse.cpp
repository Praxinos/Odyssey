// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
FOdysseyHUDEllipse::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateEllipsePoints(::ULIS::FVec2I(mCenter.X, mCenter.Y), mXRadius, mYRadius, points);

    if (points.Size() < 2)
        return;

    InitDrawCustomizedLine(iParams.mCanvas, customization, FLinearColor::Black);

    for (int i = 1; i < points.Size(); i++)
    {
        FVector2D startPoint = iParams.mTextureToHUD.Execute(FVector2D(points[i - 1].x, points[i - 1].y));
        FVector2D endPoint = iParams.mTextureToHUD.Execute(FVector2D(points[i].x, points[i].y));

        DrawCustomizedLine(
            startPoint,
            endPoint
        );
    }

    FVector2D startPoint = iParams.mTextureToHUD.Execute(FVector2D(points[points.Size() - 1].x, points[points.Size() - 1].y));
    FVector2D endPoint = iParams.mTextureToHUD.Execute(FVector2D(points[0].x, points[0].y));

    DrawCustomizedLine(
        startPoint,
        endPoint
    );

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
