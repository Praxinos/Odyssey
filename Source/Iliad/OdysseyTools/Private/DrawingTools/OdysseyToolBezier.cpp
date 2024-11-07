// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolBezier.h"
/*
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void UOdysseyToolBezier::Init()
{
    mBezier = NewObject<UOdysseyHUDBezier>();
    mIsEndPointSet = false;
    mIsControlPointSet = false;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void UOdysseyToolBezier::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    mBezier->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyToolBezier::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mBezier->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
    {
        if ( !mIsEndPointSet )
            mBezier->mEndPoint.Set( iX, iY );
        else
            mBezier->mControlPoint.Set( iX, iY );
    }
}

FReply UOdysseyToolBezier::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    mBezier->InputKey(iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

    if (iKey == EKeys::LeftMouseButton)
    {
        if ( !mIsEndPointSet )
        {
            mIsEndPointSet = true;
            ioReply = FReply::Handled();
        }
        else if ( !mIsControlPointSet )
        {
            mIsControlPointSet = true;
            ioReply = FReply::Handled();
        }
        else
        {
            if( !mIsReadyToBeApplied )
            {
                mIsReadyToBeApplied = true;
                ioReply = FReply::Handled();
            }
        }
    }

    return ioReply;
}

void UOdysseyToolBezier::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mBezier->CapturedMouseMove(iViewport, iX, iY);

    if ( !mIsReadyToBeApplied )
    {
        if ( !mIsEndPointSet )
            mBezier->mEndPoint.Set( iX, iY );
        else
            mBezier->mControlPoint.Set( iX, iY );
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> UOdysseyToolBezier::GenerateToolPoints()
{
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateQuadraticBezierPoints(
        ::ULIS::FVec2I( mBezier->mStartPoint.X, mBezier->mStartPoint.Y),
        ::ULIS::FVec2I( mBezier->mControlPoint.X, mBezier->mControlPoint.Y ),
        ::ULIS::FVec2I( mBezier->mEndPoint.X, mBezier->mEndPoint.Y ),
        1.f,
        pointsArray
    );

    return pointsArray;
} */