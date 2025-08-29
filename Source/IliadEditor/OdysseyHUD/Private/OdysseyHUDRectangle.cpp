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
FOdysseyHUDRectangle::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    FVector2D topLeft = iParams.mTextureToHUD.Execute(mTopLeftPoint);
    FVector2D topRight = iParams.mTextureToHUD.Execute(FVector2D(mBottomRightPoint.X, mTopLeftPoint.Y));
    FVector2D bottomRight = iParams.mTextureToHUD.Execute(mBottomRightPoint);
    FVector2D bottomLeft = iParams.mTextureToHUD.Execute(FVector2D(mTopLeftPoint.X, mBottomRightPoint.Y));

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

    TArray<FLinearColor> colors = customization.mColors;
    if (colors.Num() == 0)
        colors.Add(FLinearColor::Black);

    int colorIndex = 0;
    int numColors = colors.Num();
    float cumulLength = 0.f;

    // Total pattern length (Segment + Gap)
    float patternLength = customization.mSegmentLength + customization.mGapLength;

    double time = FApp::GetCurrentTime();
    float timeOffset = FMath::Fmod(time * customization.mSpeed, patternLength);

    DrawCustomizedLine(iParams.mCanvas,
        topLeft,
        topRight,
        timeOffset,
        patternLength,
        cumulLength,
        colorIndex,
        colors,
        customization,
        batchedElements
    );

    DrawCustomizedLine(iParams.mCanvas,
        topRight,
        bottomRight,
        timeOffset,
        patternLength,
        cumulLength,
        colorIndex,
        colors,
        customization,
        batchedElements
    );

    DrawCustomizedLine(iParams.mCanvas,
        bottomRight,
        bottomLeft,
        timeOffset,
        patternLength,
        cumulLength,
        colorIndex,
        colors,
        customization,
        batchedElements
    );

    DrawCustomizedLine(iParams.mCanvas,
        bottomLeft,
        topLeft,
        timeOffset,
        patternLength,
        cumulLength,
        colorIndex,
        colors,
        customization,
        batchedElements
    );

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
