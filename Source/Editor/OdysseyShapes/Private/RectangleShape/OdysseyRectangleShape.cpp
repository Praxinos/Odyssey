// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "RectangleShape/OdysseyRectangleShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDRectangle.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyRectangleShape::~UOdysseyRectangleShape()
{
}

UOdysseyRectangleShape::UOdysseyRectangleShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
{
    Step = 1.0f;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyRectangleShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun )
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mRectangle = new FOdysseyHUDRectangle(FName("Rectangle"), FVector2D( iPointInTexture.x, iPointInTexture.y), FVector2D( iPointInTexture.x, iPointInTexture.y) );
        mHUD->AddElement(mRectangle);

        FOdysseyHUDHandle* handleTopLeft = new FOdysseyHUDHandle(FName("handleTopLeft"), mRectangle, &(mRectangle->mTopLeftPoint));
        FOdysseyHUDHandle* handleBottomRight = new FOdysseyHUDHandle(FName("handleBottomRight"), mRectangle, &(mRectangle->mBottomRightPoint));

        mRectangle->AddElement(handleTopLeft);
        mRectangle->AddElement(handleBottomRight);

        mHUD->OnKeyDown(iPointInTexture, iKey);
        return true;
    }

    return false;
}

bool
UOdysseyRectangleShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mHasStrokeBegun )
    {
        mHUD->OnKeyUp(iPointInTexture, iKey);
        CommitRectangle();
        return true;
    }
    return false;
}

void
UOdysseyRectangleShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove(iPointInTexture);

    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyRectangleShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun )
    {
        mHUD->CapturedMouseMove(iPointInTexture);

        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyRectangleShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyRectangleShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

void UOdysseyRectangleShape::CommitRectangle()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateRectanglePoints(::ULIS::FVec2I(mRectangle->mTopLeftPoint.X, mRectangle->mTopLeftPoint.Y), ::ULIS::FVec2I(mRectangle->mBottomRightPoint.X, mRectangle->mBottomRightPoint.Y), pointsArray);

        FOdysseyPoint pointToAdd = FOdysseyPoint::DefaultPoint();
        for (float i = 0.f; i < pointsArray.Size(); i+=Step)
        {
            pointToAdd.x = pointsArray[i].x;
            pointToAdd.y = pointsArray[i].y;
            mRawStroke.Add(pointToAdd);
        }
        if (mRawStroke.Num() != 0)
        {
            mOnPathBeginDelegate.Broadcast(mRawStroke[0]);
            mOnPathToDelegate.Broadcast(mRawStroke);
            mOnPathEndDelegate.Broadcast({ mRawStroke.Last() });
        }
        else
        {
            AbortShape();
        }
        mHasStrokeBegun = false;
    }
}

bool UOdysseyRectangleShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        return UOdysseyShape::AbortShape();
    }
    return false;
}
