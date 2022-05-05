// IDDN FR.001.250001.005.S.P.2019.000.00000
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

void UOdysseyHUDHandle::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    UOdysseyHUDElement::MouseMove( iViewport, iX, iY );

    FVector2D pointToCheck = mPreviousTransform.TransformPoint( *mReferencePoint );

    float distSquared = FVector2D::DistSquared(pointToCheck, FVector2D(iX, iY));
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

FReply UOdysseyHUDHandle::InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply )
{
    UOdysseyHUDElement::InputKey( iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

    if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed)
    {
        FVector2D pointToCheck = mPreviousTransform.TransformPoint(*mReferencePoint);

        float distSquared = FVector2D::DistSquared(pointToCheck, FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()));
        if( distSquared < 25 )
        { 
            mIsCaptured = true;
            ioReply = FReply::Handled();
        }
    }
    else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released && mIsCaptured)
    {
        mIsCaptured = false;
        ioReply = FReply::Handled();
    }

    return ioReply;
}

void UOdysseyHUDHandle::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    UOdysseyHUDElement::CapturedMouseMove( iViewport, iX, iY );

    if (mIsCaptured)
    {
        FVector2D position = mPreviousTransform.Inverse().TransformPoint( FVector2D(iX, iY) );
        mReferencePoint->Set(position.X, position.Y);
        mIsInvalid = true;
    }
}
