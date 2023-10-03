// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "BezierShape/OdysseyBezierShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDBezier.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyBezierShape::~UOdysseyBezierShape()
{
}

UOdysseyBezierShape::UOdysseyBezierShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
    , mHasControlBegun( false )
{
    Step = 1.0f;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyBezierShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun )
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mBezier = new FOdysseyHUDBezier(FName("Bezier"), FVector2D(iPointInTexture.x, iPointInTexture.y), FVector2D(iPointInTexture.x, iPointInTexture.y), FVector2D(iPointInTexture.x, iPointInTexture.y) );
        mHUD->AddElement(mBezier);

        FOdysseyHUDHandle* handleStart = new FOdysseyHUDHandle(FName("handleStart"), mBezier, &(mBezier->mStartPoint));
        FOdysseyHUDHandle* handleControl = new FOdysseyHUDHandle(FName("handleControl"), mBezier, &(mBezier->mControlPoint));
        FOdysseyHUDHandle* handleEnd = new FOdysseyHUDHandle(FName("handleEnd"), mBezier, &(mBezier->mEndPoint));

        mBezier->AddElement(handleStart);
        mBezier->AddElement(handleControl);
        mBezier->AddElement(handleEnd);

        mHandles.Add(handleStart);
        mHandles.Add(handleControl);
        mHandles.Add(handleEnd);

        mHUD->OnKeyDown(iPointInTexture, iKey);
        return true;
    }

    return false;
}

bool
UOdysseyBezierShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mHasControlBegun)
    {
        CommitBezier();
    }
    else if( mHasStrokeBegun )
    {
        mHUD->OnKeyUp(iPointInTexture, iKey);
        if ( mHandles[2]->GetPosition() == mHandles[0]->GetPosition() )
        {
            mHandles[2]->Capture();
            return true;
        }
        else
        {
            mHasControlBegun = true;
            mHandles[1]->Capture();
            return true;
        }
    }

    mHUD->OnKeyUp(iPointInTexture, iKey);

    return false;
}

void
UOdysseyBezierShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (mHasStrokeBegun || mHasControlBegun)
    {
        mHUD->CapturedMouseMove(iPointInTexture);
    }
    else
    {
        mHUD->MouseMove(iPointInTexture);
    }
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyBezierShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun || mHasControlBegun )
    {
        mHUD->CapturedMouseMove(iPointInTexture);

        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyBezierShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyBezierShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

void UOdysseyBezierShape::CommitBezier()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateQuadraticBezierPoints(::ULIS::FVec2I(mBezier->mStartPoint.X, mBezier->mStartPoint.Y), ::ULIS::FVec2I(mBezier->mControlPoint.X, mBezier->mControlPoint.Y), ::ULIS::FVec2I(mBezier->mEndPoint.X, mBezier->mEndPoint.Y), 1.f, pointsArray);

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
        mHasControlBegun = false;
        mHandles.Empty();
    }
}

bool UOdysseyBezierShape::AbortShape()
{
    if( mHasStrokeBegun || mHasControlBegun )
    {
        mHasStrokeBegun = false;
        mHasControlBegun = false;
        mHandles.Empty();
        return UOdysseyShape::AbortShape();
    }
    return false;
}
