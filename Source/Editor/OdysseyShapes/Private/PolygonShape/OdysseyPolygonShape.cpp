// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "PolygonShape/OdysseyPolygonShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDPolygon.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPolygonShape::~UOdysseyPolygonShape()
{
}

UOdysseyPolygonShape::UOdysseyPolygonShape(const FObjectInitializer& iObjectInitializer)
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
UOdysseyPolygonShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun )
    {
        return true;
    }

    return false;
}

bool
UOdysseyPolygonShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mHasStrokeBegun )
    {
        mHUD->OnKeyUp(iPointInTexture, iKey);
        CommitPolygon();
        return true;
    }
    return false;
}

void
UOdysseyPolygonShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove(iPointInTexture);

    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyPolygonShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun )
    {
        mHUD->CapturedMouseMove(iPointInTexture);

        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyPolygonShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyPolygonShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

void UOdysseyPolygonShape::CommitPolygon()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        //::ULIS::GeneratePolygonPoints(::ULIS::FVec2I(mPolygon->mStartPoint.X, mPolygon->mStartPoint.Y), ::ULIS::FVec2I(mPolygon->mFinishPoint.X, mPolygon->mFinishPoint.Y), pointsArray);

        FOdysseyPoint pointToAdd = FOdysseyPoint::DefaultPoint();
        for (float i = 0.f; i < pointsArray.Size() - 1; i+=Step)
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

bool UOdysseyPolygonShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mOnPathResetDelegate.Broadcast();
        mOnPathAbortDelegate.Broadcast();
        mHasStrokeBegun = false;
        return true;
    }
    return false;
}
