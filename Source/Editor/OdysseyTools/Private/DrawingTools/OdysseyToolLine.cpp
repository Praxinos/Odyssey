// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolLine.h"
/*
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void UOdysseyToolLine::Init()
{
    mLine = NewObject<UOdysseyHUDLine>();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void UOdysseyToolLine::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    mLine->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyToolLine::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mLine->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
        mLine->mFinishPoint.Set( iX, iY );
}

FReply UOdysseyToolLine::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    mLine->InputKey(iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

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

void UOdysseyToolLine::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mLine->CapturedMouseMove(iViewport, iX, iY);

    if ( !mIsReadyToBeApplied )
    {
        mLine->mFinishPoint.Set(iX, iY);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> UOdysseyToolLine::GenerateToolPoints()
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateLinePoints( ::ULIS::FVec2I( mLine->mStartPoint.X, mLine->mStartPoint.Y), ::ULIS::FVec2I( mLine->mFinishPoint.X, mLine->mFinishPoint.Y ), pointsArray );

    return pointsArray;
}*/