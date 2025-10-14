// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "LineShape/OdysseyLineShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDLine.h"
#include "OdysseyHUDElement.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyLineShape::~UOdysseyLineShape()
{
}

UOdysseyLineShape::UOdysseyLineShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyLineShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mIsDrawing = true;

    mOnBegin.Broadcast();

    //Line Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    mStartPoint = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
    mEndPoint = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);

    CreateHUD();

    return true;
}

void
UOdysseyLineShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsDrawing)
        return;

    //Line Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    mEndPoint = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);

    bool snapAngles = SnapAngles ^ mInvertSnapAngles;
    if( snapAngles )
    {
        int shiftX = FMath::Abs( mEndPoint.x - mStartPoint.x);
        int shiftY = FMath::Abs( mEndPoint.y - mStartPoint.y);

        if( shiftX > shiftY )
            mEndPoint.y = mStartPoint.y;
        else
            mEndPoint.x = mStartPoint.x;
    }

    RefreshHUD();

    mOnInteractive.Broadcast( { mStartPoint, mEndPoint } );

    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyLineShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsDrawing)
        return UOdysseyShape::OnMouseUp(iPointInTexture, iKey);

    mIsDrawing = false;
    RemoveHUD();
    mOnCommit.Broadcast( { mStartPoint, mEndPoint }, true);
    return true;
}

bool
UOdysseyLineShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return true;
    }

    if( iKey == EKeys::LeftShift || iKey == EKeys::RightShift )
    {
        mInvertSnapAngles = true;
        return true;
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyLineShape::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        mInvertSnapAngles = false;
        return true;
    }

    return UOdysseyShape::OnKeyUp(iKey);
}

void
UOdysseyLineShape::Abort()
{
    mIsDrawing = false;

    RemoveHUD();

    mOnAbort.Broadcast();
}

void
UOdysseyLineShape::CreateHUD()
{
    mLineHUD = MakeShared<FOdysseyHUDLine>(mStartPoint, mEndPoint);

    mHandleStartHUD = MakeShared<FOdysseyHUDHandle>(mStartPoint);
    mHandleEndHUD = MakeShared<FOdysseyHUDHandle>(mEndPoint);
    mHandleStartHUD->IsInteractable(false);
    mHandleEndHUD->IsInteractable(false);

    mHUD->AddElement(mLineHUD);
    mLineHUD->AddElement(mHandleStartHUD);
    mLineHUD->AddElement(mHandleEndHUD);
}

void
UOdysseyLineShape::RefreshHUD()
{
    mLineHUD->SetEndPoint(mEndPoint);
    mHandleEndHUD->SetPosition(mEndPoint);
}

void
UOdysseyLineShape::RemoveHUD()
{
    mHUD->RemoveElement(mLineHUD);
    mLineHUD = nullptr;
    mHandleStartHUD = nullptr;
    mHandleEndHUD = nullptr;
}
