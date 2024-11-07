// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolRectangle.h"
/*
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void UOdysseyToolRectangle::Init()
{
    mRectangle = NewObject<UOdysseyHUDRectangle>();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void UOdysseyToolRectangle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    mLastTransform = iTransform;
    mRectangle->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyToolRectangle::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mRectangle->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
        mRectangle->mBottomRightPoint.Set( iX, iY );
}

FReply UOdysseyToolRectangle::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    mRectangle->InputKey(iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

    if (iKey == EKeys::LeftMouseButton)
    {
        if( !mIsReadyToBeApplied )
        {
            mIsReadyToBeApplied = true;
            ioReply = FReply::Handled();
        }
    }

    return ioReply;
}

void UOdysseyToolRectangle::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mRectangle->CapturedMouseMove(iViewport, iX, iY);

    if ( !mIsReadyToBeApplied )
    {
        mRectangle->mBottomRightPoint.Set(iX, iY);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> UOdysseyToolRectangle::GenerateToolPoints()
{
    float angle = 360 + ::ULIS::FMath::RadToDeg(mLastTransform.GetMatrix().GetRotationAngle()*2);
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateCircleInscribedRectanglePoints( ::ULIS::FVec2I( mRectangle->mTopLeftPoint.X, mRectangle->mTopLeftPoint.Y), ::ULIS::FVec2I( mRectangle->mBottomRightPoint.X, mRectangle->mBottomRightPoint.Y ), angle, pointsArray );

    return pointsArray;
}
*/