// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDHandle.h"

#include "ULISLoaderModule.h"

#define HANDLE_SMALL_SIZE 10
#define HANDLE_BIG_SIZE 20

FOdysseyHUDHandle::~FOdysseyHUDHandle()
{

}

FOdysseyHUDHandle::FOdysseyHUDHandle(FName iName, FOdysseyHUDElement* iParent, FVector2D* iReferencePoint, FTransform2D iTransform /*= FTransform2D() */) :
    FOdysseyHUDElement(iName, iTransform)
{
    mParent = iParent;
    mPreviousHandleSize = mHandleSize = HANDLE_SMALL_SIZE;
    mReferencePoint = iReferencePoint;
    mPreviousPosition = *mReferencePoint;
}

void FOdysseyHUDHandle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;

    if ( mIsInvalid || mPreviousTransform != iTransform || mPreviousHandleSize != mHandleSize || mPreviousPosition != *mReferencePoint )
    {
        Erase(ioBlock, iTransform);
        //Draw the children of this HUDElement
        FOdysseyHUDElement::Draw(ioBlock, iTransform);
    }
    else
    {
        //Draw the children of this HUDElement
        FOdysseyHUDElement::Draw(ioBlock, iTransform);
        return;
    }

    FVector2D transformedReferencePoint = iTransform.TransformPoint(*mReferencePoint);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawCircle(*(ioBlock), ::ULIS::FVec2I(transformedReferencePoint.X, transformedReferencePoint.Y),  mHandleSize / 2.f, ::ULIS::FColor::FromRGBA8(255, 0, 0, 255));
    ctx.Finish();

    mPreviousTransform = iTransform;
    mPreviousHandleSize = mHandleSize;
    mPreviousPosition = *mReferencePoint;

    ioBlock->Dirty();
}

void FOdysseyHUDHandle::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    //Erase the children of this HUDElement
    FOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedReferencePoint = mPreviousTransform.TransformPoint(mPreviousPosition);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawCircle(*(ioBlock), ::ULIS::FVec2I(transformedReferencePoint.X, transformedReferencePoint.Y),  mPreviousHandleSize / 2.f, ::ULIS::FColor::FromRGBA8(0, 0, 0, 0));
    ctx.Finish();
}

void FOdysseyHUDHandle::SetPosition(FVector2D iNewPosition)
{
    if( mReferencePoint )
        mReferencePoint->Set( iNewPosition.X, iNewPosition.Y );
}

FVector2D FOdysseyHUDHandle::GetPosition()
{
    if( mReferencePoint )
        return *mReferencePoint;

    return FVector2D( -1, -1 );
}

void FOdysseyHUDHandle::MouseMove( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyHUDElement::MouseMove(iPointInTexture);

    float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iPointInTexture.x, iPointInTexture.y));
    if ( mHandleSize != HANDLE_BIG_SIZE && distSquared < HANDLE_BIG_SIZE * HANDLE_BIG_SIZE)
    {
        mHandleSize = HANDLE_BIG_SIZE;
        mIsInvalid = true;
    }
    else if( mHandleSize != HANDLE_SMALL_SIZE && distSquared > HANDLE_BIG_SIZE * HANDLE_BIG_SIZE )
    {
        mHandleSize = HANDLE_SMALL_SIZE;
        mIsInvalid = true;
    }
}

bool FOdysseyHUDHandle::OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey )
{
    FOdysseyHUDElement::OnKeyDown( iPointInTexture, iKey );

    if (iKey == EKeys::LeftMouseButton)
    {
        float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iPointInTexture.x, iPointInTexture.y));
        if( distSquared < HANDLE_BIG_SIZE * HANDLE_BIG_SIZE )
        { 
            mIsCaptured = true;
            return true;
        }
    }

    return false;
}

bool FOdysseyHUDHandle::OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey )
{
    FOdysseyHUDElement::OnKeyUp(iPointInTexture, iKey);

    if (iKey == EKeys::LeftMouseButton && mIsCaptured)
    {
        mIsCaptured = false;
        return true;
    }

    return false;
}

void FOdysseyHUDHandle::CapturedMouseMove( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyHUDElement::CapturedMouseMove( iPointInTexture );

    if (mIsCaptured)
    {
        mReferencePoint->Set(iPointInTexture.x, iPointInTexture.y);
        mIsInvalid = true;
    }
}
