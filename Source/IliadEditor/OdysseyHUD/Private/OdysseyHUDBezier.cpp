// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDBezier.h"

#include "BatchedElements.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDBezier::~FOdysseyHUDBezier()
{

}

FOdysseyHUDBezier::FOdysseyHUDBezier(const FVector2D& iStartPoint, const FVector2D& iEndPoint, const FVector2D& iControlPoint)
    : mStartPoint(iStartPoint)
    , mEndPoint(iEndPoint)
    , mControlPoint(iControlPoint)
{
}

void
FOdysseyHUDBezier::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    // If we passed a customization in iParams, we use this one, else, we use the one that is in FOdysseyHUD
    const FOdysseyHUDElement::FHUDCustomization& customization = iParams.mCustomization ? *iParams.mCustomization : mCustomization;

    if (customization.mSegmentLength <= 0.f || customization.mGapLength < 0.f)
        return;

    FVector2D startPoint = mStartPoint;
    FVector2D controlPoint = mControlPoint;
    FVector2D endPoint = mEndPoint;

    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateQuadraticBezierPoints(
        ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y),
        ::ULIS::FVec2I(mControlPoint.X, mControlPoint.Y),
        ::ULIS::FVec2I(mEndPoint.X, mEndPoint.Y),
        1.0f,
        pointsArray
    );

    if (pointsArray.Size() < 2)
        return;

    InitDrawCustomizedLine(iParams.mCanvas, customization, FLinearColor::Black);

    for (int i = 1; i < pointsArray.Size(); i++)
    {
        FVector2D startBezierPoint( pointsArray[i - 1].x, pointsArray[i - 1].y);
        FVector2D endBezierPoint( pointsArray[i].x, pointsArray[i].y);

        // Bezier
        DrawCustomizedLine(
            iParams,
            startBezierPoint,
            endBezierPoint
        );
    }

    // Control lines
    DrawCustomizedLine(
        iParams,
        startPoint,
        controlPoint
    );

    DrawCustomizedLine(
        iParams,
        controlPoint,
        endPoint
    );

    FOdysseyHUDElement::DrawHUD(iParams);
}

void
FOdysseyHUDBezier::SetStartPoint(const FVector2D& iPoint)
{
    mStartPoint = iPoint;
}

void
FOdysseyHUDBezier::SetEndPoint(const FVector2D& iPoint)
{
    mEndPoint = iPoint;
}

void
FOdysseyHUDBezier::SetControlPoint(const FVector2D& iPoint)
{
    mControlPoint = iPoint;
}

const FVector2D&
FOdysseyHUDBezier::GetStartPoint() const
{
    return mStartPoint;
}

const FVector2D&
FOdysseyHUDBezier::GetEndPoint() const
{
    return mEndPoint;
}

const FVector2D&
FOdysseyHUDBezier::GetControlPoint() const
{
    return mControlPoint;
}
