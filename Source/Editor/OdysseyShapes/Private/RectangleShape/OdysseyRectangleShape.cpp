// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "RectangleShape/OdysseyRectangleShape.h"

#include "OdysseyHUDHandle.h"
#include "OdysseyHUDRectangle.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyRectangleShape::~UOdysseyRectangleShape()
{
}

UOdysseyRectangleShape::UOdysseyRectangleShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyRectangleShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mIsDrawing = true;

    //Rectangle Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    mTopLeftPoint = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
    mBottomRightPoint = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);

    CreateHUD();
    return true;
}

void
UOdysseyRectangleShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsDrawing)
        return;

    //Rectangle Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    mBottomRightPoint = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);
    if (mUniform)
    {
        int shiftX = mBottomRightPoint.x - mTopLeftPoint.x;
        int shiftY = mBottomRightPoint.y - mTopLeftPoint.y;

        int signX = shiftX < 0 ? -1 : 1;
        int signY = shiftY < 0 ? -1 : 1;

        int mult = signX == signY ? 1 : -1;

        if( FMath::Abs(shiftX) > FMath::Abs(shiftY) )
        {
            mBottomRightPoint.x = mTopLeftPoint.x + shiftX;
            mBottomRightPoint.y = mTopLeftPoint.y + shiftX * mult;
        }
        else
        {
            mBottomRightPoint.x = mTopLeftPoint.x + shiftY * mult;
            mBottomRightPoint.y = mTopLeftPoint.y + shiftY;
        }
    }

    RefreshHUD();

    FOdysseyPoint topRightPoint(mBottomRightPoint.x, mTopLeftPoint.y);
    FOdysseyPoint bottomLeftPoint(mTopLeftPoint.x, mBottomRightPoint.y);
    mOnInteractive.Broadcast( { mTopLeftPoint, topRightPoint, mBottomRightPoint, bottomLeftPoint, mTopLeftPoint }, true );
    
    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyRectangleShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsDrawing)
        return UOdysseyShape::OnMouseUp(iPointInTexture, iKey);

    mIsDrawing = false;

    RemoveHUD();

    FOdysseyPoint topRightPoint(mBottomRightPoint.x, mTopLeftPoint.y);
    FOdysseyPoint bottomLeftPoint(mTopLeftPoint.x, mBottomRightPoint.y);
    mOnCommit.Broadcast( { mTopLeftPoint, topRightPoint, mBottomRightPoint, bottomLeftPoint, mTopLeftPoint }, true);

    return true;
}

bool
UOdysseyRectangleShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return true;
    }

    if( iKey == EKeys::LeftShift || iKey == EKeys::RightShift )
	{
		mUniform = true;
		return true;
	}

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyRectangleShape::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        mUniform = false;
		return true;
    }

    return UOdysseyShape::OnKeyUp(iKey);
}

void
UOdysseyRectangleShape::Abort()
{
    mIsDrawing = false;

    RemoveHUD();

    mOnAbort.Broadcast();
}

void
UOdysseyRectangleShape::CreateHUD()
{   
    mRectangleHUD = MakeShared<FOdysseyHUDRectangle>(mTopLeftPoint, mBottomRightPoint);

    mHandleTopLeftHUD = MakeShared<FOdysseyHUDHandle>(mTopLeftPoint);
    mHandleBottomRightHUD = MakeShared<FOdysseyHUDHandle>(mBottomRightPoint);
    mHandleTopLeftHUD->IsInteractable(false);
    mHandleBottomRightHUD->IsInteractable(false);

    mHUD->AddElement(mRectangleHUD);
    mRectangleHUD->AddElement(mHandleTopLeftHUD);
    mRectangleHUD->AddElement(mHandleBottomRightHUD);
}

void
UOdysseyRectangleShape::RefreshHUD()
{
    mRectangleHUD->SetBottomRightPoint(mBottomRightPoint);
    mHandleBottomRightHUD->SetPosition(mBottomRightPoint);
}

void
UOdysseyRectangleShape::RemoveHUD()
{
    mHUD->RemoveElement(mRectangleHUD);
    mRectangleHUD = nullptr;
    mHandleTopLeftHUD = nullptr;
    mHandleBottomRightHUD = nullptr;
}
