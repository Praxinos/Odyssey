// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDHandle.h"


void UOdysseyHUDHandle::Init( FName iName, UOdysseyHUDElement* iParent, FVector2D* iReferencePoint, FTransform2D iTransform )
{
    UOdysseyHUDElement::Init( iName, iTransform );
    mParent = iParent;
    mPreviousHandleSize = mHandleSize = 2;
    mReferencePoint = iReferencePoint;
    mPreviousPosition = *mReferencePoint;
}

TSharedPtr<SWidget> UOdysseyHUDHandle::CreateWidget()
{
    UOdysseyHUDElement::CreateWidget();

    return mElementsWidget;
}

void UOdysseyHUDHandle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    if (!ioBlock)
        return;

    if ( mIsInvalid || mPreviousTransform != iTransform || mPreviousHandleSize != mHandleSize || mPreviousPosition != *mReferencePoint )
    {
        Erase(ioBlock, iTransform);
        //Draw the children of this HUDElement
        UOdysseyHUDElement::Draw(ioBlock, iTransform);
    }
    else
    {
        //Draw the children of this HUDElement
        UOdysseyHUDElement::Draw(ioBlock, iTransform);
        return;
    }

    FVector2D transformedReferencePoint = iTransform.TransformPoint(*mReferencePoint);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(ioBlock), ::ULIS::FVec2I(transformedReferencePoint.X - mHandleSize, transformedReferencePoint.Y - mHandleSize), ::ULIS::FVec2I(transformedReferencePoint.X + mHandleSize, transformedReferencePoint.Y + mHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 255));
    ctx.Finish();

    mPreviousTransform = iTransform;
    mPreviousHandleSize = mHandleSize;
    mPreviousPosition = *mReferencePoint;

    ioBlock->Dirty();
}

void UOdysseyHUDHandle::Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    //Erase the children of this HUDElement
    UOdysseyHUDElement::Erase(ioBlock, iTransform);

    FVector2D transformedReferencePoint = mPreviousTransform.TransformPoint(mPreviousPosition);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.DrawRectangle(*(ioBlock), ::ULIS::FVec2I(transformedReferencePoint.X - mPreviousHandleSize, transformedReferencePoint.Y - mPreviousHandleSize), ::ULIS::FVec2I(transformedReferencePoint.X + mPreviousHandleSize, transformedReferencePoint.Y + mPreviousHandleSize), ::ULIS::FColor::RGBA8(255, 0, 0, 0));
    ctx.Finish();
}

void UOdysseyHUDHandle::SetPosition(FVector2D iNewPosition)
{
    if( mReferencePoint )
        mReferencePoint->Set( iNewPosition.X, iNewPosition.Y );
}

FVector2D UOdysseyHUDHandle::GetPosition()
{
    if( mReferencePoint )
        return *mReferencePoint;

    return FVector2D( -1, -1 );
}

void UOdysseyHUDHandle::MouseMove( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyHUDElement::MouseMove(iPointInTexture);

    float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iPointInTexture.x, iPointInTexture.y));
    if ( mHandleSize != 5 && distSquared < 25)
    {
        mHandleSize = 5;
        mIsInvalid = true;
    }
    else if( mHandleSize != 2 && distSquared > 25 )
    {
        mHandleSize = 2;
        mIsInvalid = true;
    }
}

bool UOdysseyHUDHandle::OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey )
{
    UOdysseyHUDElement::OnKeyDown( iPointInTexture, iKey );

    if (iKey == EKeys::LeftMouseButton)
    {
        float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iPointInTexture.x, iPointInTexture.y));
        if( distSquared < 25 )
        { 
            mIsCaptured = true;
            return true;
        }
    }

    return false;
}

bool UOdysseyHUDHandle::OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey )
{
    UOdysseyHUDElement::OnKeyUp(iPointInTexture, iKey);

    if (iKey == EKeys::LeftMouseButton && mIsCaptured)
    {
        mIsCaptured = false;
        return true;
    }

    return false;
}

void UOdysseyHUDHandle::CapturedMouseMove( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyHUDElement::CapturedMouseMove( iPointInTexture );

    if (mIsCaptured)
    {
        mReferencePoint->Set(iPointInTexture.x, iPointInTexture.y);
        mIsInvalid = true;
    }
}
