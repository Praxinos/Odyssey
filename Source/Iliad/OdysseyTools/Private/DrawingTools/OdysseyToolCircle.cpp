// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolCircle.h"
/*
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void UOdysseyToolCircle::Init()
{
    mCircle = NewObject<UOdysseyHUDCircle>();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void UOdysseyToolCircle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    mCircle->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyToolCircle::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mCircle->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
    {
        mCircle->mBorderPoint.Set( iX, iY );
        mCircle->mRadius = (int)::ULIS::FMath::Dist( mCircle->mCenterPoint.X, mCircle->mCenterPoint.Y, mCircle->mBorderPoint.X, mCircle->mBorderPoint.Y );
    }
}

FReply UOdysseyToolCircle::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    mCircle->InputKey(iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

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

void UOdysseyToolCircle::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mCircle->CapturedMouseMove(iViewport, iX, iY);

    if ( !mIsReadyToBeApplied )
    {
        mCircle->mBorderPoint.Set(iX, iY);
        mCircle->mRadius = (int)::ULIS::FMath::Dist( mCircle->mCenterPoint.X, mCircle->mCenterPoint.Y, mCircle->mBorderPoint.X, mCircle->mBorderPoint.Y );
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> UOdysseyToolCircle::GenerateToolPoints()
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateCirclePoints( ::ULIS::FVec2I( mCircle->mCenterPoint.X, mCircle->mCenterPoint.Y), mCircle->mRadius, pointsArray );

    return pointsArray;
} */