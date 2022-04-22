// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyToolBezier.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyToolBezier::~FOdysseyToolBezier()
{
}

FOdysseyToolBezier::FOdysseyToolBezier( FVector2D iStartPoint )
    : mBezier(NewObject<UOdysseyHUDBezier>())
    , mIsEndPointSet( false )
    , mIsControlPointSet( false )
{
    mBezier->Init( FName("bezierTool"), iStartPoint, iStartPoint, iStartPoint );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void FOdysseyToolBezier::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    mBezier->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void FOdysseyToolBezier::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
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

FReply FOdysseyToolBezier::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
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

void FOdysseyToolBezier::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
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

::ULIS::TArray<::ULIS::FVec2I> FOdysseyToolBezier::GenerateToolPoints()
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
}