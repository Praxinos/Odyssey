// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "BezierShape/OdysseyBezierShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDBezier.h"

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
    if( mEventState != eEventState::Idle )
        return false;
    
    mEventState = eEventState::EndPoint;

    //Bezier Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    FOdysseyPoint point = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
    mStartPoint = point;
    mControlPoint = point;
    mEndPoint = point;

    CreateHUD();

    return true;   
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

        mOnInteractive.Broadcast(GeneratePoints(), true);
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

        mOnInteractive.Broadcast(GeneratePoints(), true);
    }

    if ( mEventState == eEventState::ControlPoint )
    {
        //Bezier Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        FOdysseyPoint point = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
        mControlPoint = point;

        RefreshHUD();

        mOnInteractive.Broadcast(GeneratePoints(), true);
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