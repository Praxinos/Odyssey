// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyToolRectangle.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyToolRectangle::~FOdysseyToolRectangle()
{
}

FOdysseyToolRectangle::FOdysseyToolRectangle( FVector2D iTopLeftPoint )
    : mRectangle(NewObject<UOdysseyHUDRectangle>())
{
    mRectangle->Init( FName("RectangleTool"), iTopLeftPoint, iTopLeftPoint );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void FOdysseyToolRectangle::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform /*= FTransform2D()*/)
{
    mLastTransform = iTransform;
    mRectangle->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void FOdysseyToolRectangle::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mRectangle->MouseMove(iViewport, iX, iY);

    if( !mIsReadyToBeApplied )
        mRectangle->mBottomRightPoint.Set( iX, iY );
}

FReply FOdysseyToolRectangle::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
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

void FOdysseyToolRectangle::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mRectangle->CapturedMouseMove(iViewport, iX, iY);

    if ( !mIsReadyToBeApplied )
    {
        mRectangle->mBottomRightPoint.Set(iX, iY);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> FOdysseyToolRectangle::GenerateToolPoints()
{
    float angle = 360 + ::ULIS::FMath::RadToDeg(mLastTransform.GetMatrix().GetRotationAngle()*2);
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateCircleInscribedRectanglePoints( ::ULIS::FVec2I( mRectangle->mTopLeftPoint.X, mRectangle->mTopLeftPoint.Y), ::ULIS::FVec2I( mRectangle->mBottomRightPoint.X, mRectangle->mBottomRightPoint.Y ), angle, pointsArray );

    return pointsArray;
}
