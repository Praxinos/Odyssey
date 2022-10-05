// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolEllipse.h"
/*
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void UOdysseyToolEllipse::Init()
{
    mEllipse = NewObject<UOdysseyHUDEllipse>();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void UOdysseyToolEllipse::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    mEllipse->Draw( ioBlock, iTransform );
    mPreviousTransform = iTransform;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyToolEllipse::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mEllipse->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
    {
        mEllipse->mBorderPoint.Set( iX, iY );
        mEllipse->mEllipseAaxis = FMath::Abs(mEllipse->mCenterPoint.X - mEllipse->mBorderPoint.X);
        mEllipse->mEllipseBaxis = FMath::Abs(mEllipse->mCenterPoint.Y - mEllipse->mBorderPoint.Y);
    }
}

FReply UOdysseyToolEllipse::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
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

void UOdysseyToolEllipse::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mEllipse->CapturedMouseMove(iViewport, iX, iY);
 
    if ( !mIsReadyToBeApplied )
    {
        mEllipse->mBorderPoint.Set(iX, iY);
        mEllipse->mEllipseAaxis = FMath::Abs(mEllipse->mCenterPoint.X - mEllipse->mBorderPoint.X);
        mEllipse->mEllipseBaxis = FMath::Abs(mEllipse->mCenterPoint.Y - mEllipse->mBorderPoint.Y);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> UOdysseyToolEllipse::GenerateToolPoints()
{
    FTransform2D rotation = FTransform2D(FQuat2D(-mPreviousTransform.GetMatrix().GetRotationAngle()));
    FVector2D transformedBorderPoint = mEllipse->mBorderPoint - mEllipse->mCenterPoint;
    transformedBorderPoint = rotation.TransformPoint(transformedBorderPoint);
    transformedBorderPoint += mEllipse->mCenterPoint;

    UE_LOG(LogTemp, Display, TEXT("normal: %lf, %lf"), mEllipse->mBorderPoint.X, mEllipse->mBorderPoint.Y)
    UE_LOG(LogTemp, Display, TEXT("Transformed by %lf: %lf, %lf"), mPreviousTransform.GetMatrix().GetRotationAngle(), transformedBorderPoint.X, transformedBorderPoint.Y)


    int transformedEllipseAaxis = FMath::Abs(mEllipse->mCenterPoint.X - transformedBorderPoint.X);
    int transformedEllipseBaxis = FMath::Abs(mEllipse->mCenterPoint.Y - transformedBorderPoint.Y);
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateRotatedEllipsePoints( ::ULIS::FVec2I( mEllipse->mCenterPoint.X, mEllipse->mCenterPoint.Y), transformedEllipseAaxis, transformedEllipseBaxis, ULIS::FMath::RadToDeg(mPreviousTransform.GetMatrix().GetRotationAngle()), pointsArray );

    return pointsArray;
} */