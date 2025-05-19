// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "BezierShape/OdysseyBezierShape.h"
#include "OdysseyHUDBezier.h"
#include "OdysseyHUDElement.h"
#include "OdysseyHUDHandle.h"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyBezierShape::~UOdysseyBezierShape()
{
}

UOdysseyBezierShape::UOdysseyBezierShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyBezierShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mEventState == eEventState::Idle)
    {
        mEventState = eEventState::EndPoint;

        //Bezier Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        FOdysseyPoint point = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
        mStartPoint = point;
        mControlPoint = point;
        mEndPoint = point;

        CreateHUD();
        return true;
    }

    if( mEventState == eEventState::ControlPoint)
    {
        //ensures the tools keeps focus on the pointer (avoids the viewport to take control of the pointer)
        return true;
    }

    return false;
}

void
UOdysseyBezierShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if ( mEventState == eEventState::ControlPoint )
    {
        //Bezier Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        FOdysseyPoint point = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
        mControlPoint = point;

        RefreshHUD();

        mOnInteractive.Broadcast(GeneratePoints());
    }

    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyBezierShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if ( mEventState == eEventState::EndPoint )
    {
        //Bezier Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        FOdysseyPoint point = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
        mEndPoint = point;

        RefreshHUD();

        mOnInteractive.Broadcast(GeneratePoints());
    }

    if ( mEventState == eEventState::ControlPoint )
    {
        //Bezier Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        FOdysseyPoint point = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
        mControlPoint = point;

        RefreshHUD();

        mOnInteractive.Broadcast(GeneratePoints());
    }

    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyBezierShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mEventState == eEventState::EndPoint )
    {
        mEventState = eEventState::ControlPoint;
        return true;
    }

    if ( mEventState == eEventState::ControlPoint)
    {
        mOnCommit.Broadcast(GeneratePoints(), true);
        RemoveHUD();
        mEventState = eEventState::Idle;
        return true;
    }

    return false;
}

bool
UOdysseyBezierShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return true;
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

void
UOdysseyBezierShape::Abort()
{
    mEventState = eEventState::Idle;

    RemoveHUD();

    mOnAbort.Broadcast();
}

TArray<FOdysseyPoint>
UOdysseyBezierShape::GeneratePoints() const
{
    TArray<FOdysseyPoint> points = GeneratePointsFromFunction(
        [startPoint = mStartPoint, controlPoint = mControlPoint, endPoint = mEndPoint](float iValue)
        {
            return FVector2D(
                (1.f - iValue) * ( (1.f - iValue) * startPoint.x + iValue * controlPoint.x ) + iValue * ((1.f - iValue) * controlPoint.x + iValue * endPoint.x),
                (1.f - iValue) * ( (1.f - iValue) * startPoint.y + iValue * controlPoint.y ) + iValue * ((1.f - iValue) * controlPoint.y + iValue * endPoint.y)
            );
        }
    );

    return points;
}

void
UOdysseyBezierShape::CreateHUD()
{
    mBezierHUD = MakeShared<FOdysseyHUDBezier>(mStartPoint, mControlPoint, mEndPoint);

    mHandleStartHUD = MakeShared<FOdysseyHUDHandle>(mStartPoint);
    mHandleControlHUD = MakeShared<FOdysseyHUDHandle>(mControlPoint);
    mHandleEndHUD = MakeShared<FOdysseyHUDHandle>(mEndPoint);
    mHandleStartHUD->IsInteractable(false);
    mHandleControlHUD->IsInteractable(false);
    mHandleEndHUD->IsInteractable(false);

    mHUD->AddElement(mBezierHUD);
    mBezierHUD->AddElement(mHandleStartHUD);
    mBezierHUD->AddElement(mHandleControlHUD);
    mBezierHUD->AddElement(mHandleEndHUD);
}

void
UOdysseyBezierShape::RefreshHUD()
{
    mBezierHUD->SetEndPoint(mEndPoint);
    mBezierHUD->SetControlPoint(mControlPoint);
    mHandleControlHUD->SetPosition(mEndPoint);
    mHandleEndHUD->SetPosition(mControlPoint);
}

void
UOdysseyBezierShape::RemoveHUD()
{
    mHUD->RemoveElement(mBezierHUD);
    mBezierHUD = nullptr;
    mHandleStartHUD = nullptr;
    mHandleControlHUD = nullptr;
    mHandleEndHUD = nullptr;
}
