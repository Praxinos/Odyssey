// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyToolEllipse.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyToolEllipse::~FOdysseyToolEllipse()
{
}

FOdysseyToolEllipse::FOdysseyToolEllipse( FVector2D iCenterPoint )
: mEllipse(NewObject<UOdysseyHUDEllipse>())
{
    mEllipse->Init( FName("EllipseTool"), iCenterPoint, iCenterPoint, 0);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void FOdysseyToolEllipse::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    mEllipse->Draw( ioBlock, iTransform );
    mPreviousTransform = iTransform;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void FOdysseyToolEllipse::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mEllipse->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
    {
        mEllipse->mEndPoint.Set( iX, iY );
        mEllipse->mEllipseAaxis = (int)(mEllipse->mCenterPoint.X - mEllipse->mEndPoint.X);
        mEllipse->mEllipseBaxis = (int)(mEllipse->mCenterPoint.Y - mEllipse->mEndPoint.Y);
        mEllipse->mAngle = 0;
        UE_LOG(LogTemp, Display, TEXT("%lf, %lf"), mEllipse->mEndPoint.X, mEllipse->mEndPoint.Y)
    }
}

FReply FOdysseyToolEllipse::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    mEllipse->InputKey(iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

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

void FOdysseyToolEllipse::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mEllipse->CapturedMouseMove(iViewport, iX, iY);
 
    if ( !mIsReadyToBeApplied )
    {
        mEllipse->mEndPoint.Set(iX, iY);
        mEllipse->mEllipseAaxis = (int)(mEllipse->mCenterPoint.X - mEllipse->mEndPoint.X);
        mEllipse->mEllipseBaxis = (int)(mEllipse->mCenterPoint.Y - mEllipse->mEndPoint.Y);
        mEllipse->mAngle = 0;
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> FOdysseyToolEllipse::GenerateToolPoints()
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateRotatedEllipsePoints( ::ULIS::FVec2I( mEllipse->mCenterPoint.X, mEllipse->mCenterPoint.Y), FMath::Abs(mEllipse->mEllipseAaxis), FMath::Abs(mEllipse->mEllipseBaxis), ULIS::FMath::RadToDeg(mPreviousTransform.GetMatrix().GetRotationAngle()), pointsArray );

    return pointsArray;
}