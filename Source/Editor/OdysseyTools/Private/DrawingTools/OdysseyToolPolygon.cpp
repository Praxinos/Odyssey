// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyToolPolygon.h"
/*
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

void UOdysseyToolPolygon::Init()
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Drawing

void UOdysseyToolPolygon::Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform)
{
    for( UOdysseyHUDLine* lineToDraw : mLines )
        lineToDraw->Draw( ioBlock, iTransform );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyToolPolygon::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    for( UOdysseyHUDLine* line : mLines )
    {
        line->MouseMove(iViewport, iX, iY);
        line->Invalidate();
    }

    if( !mIsReadyToBeApplied )
        mLines.Last()->mFinishPoint.Set( iX, iY );
}

FReply UOdysseyToolPolygon::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    for( UOdysseyHUDLine* line : mLines )
        line->InputKey( iViewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad, ioReply );

    // Delete last line
    if ( iKey == EKeys::LeftMouseButton && ( iViewport->KeyState( EKeys::LeftAlt ) || iViewport->KeyState( EKeys::RightAlt ) ) )
    {
        if (mLines.Num() >= 1)
            mLines.Pop();

        return FReply::Handled();
    }

    // Add new line
    if ( iKey == EKeys::LeftMouseButton )
    {
        UOdysseyHUDLine* newLine = NewObject<UOdysseyHUDLine>();
        newLine->Init( FName("Line"), mLines.Last()->mFinishPoint, mLines.Last()->mFinishPoint );
        mLines.Add( newLine );
        ioReply = FReply::Handled();
    }

    // Drawing the Polygon once everything is ok
    if ( ( iEvent == EInputEvent::IE_DoubleClick && iKey == EKeys::LeftMouseButton ) || iKey == EKeys::Enter )
    {
        if( !mIsReadyToBeApplied )
        {
            mIsReadyToBeApplied = true;
            ioReply = FReply::Handled();
        }
    }

    return ioReply;
}

void UOdysseyToolPolygon::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    for( UOdysseyHUDLine* line : mLines )
    {
        line->CapturedMouseMove(iViewport, iX, iY);
        line->Invalidate();
    }

    if ( !mIsReadyToBeApplied )
        mLines.Last()->mFinishPoint.Set( iX, iY );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Path generation

::ULIS::TArray<::ULIS::FVec2I> UOdysseyToolPolygon::GenerateToolPoints()
{
    std::vector< ::ULIS::FVec2I > polygonPoints;

    for( UOdysseyHUDLine* line : mLines )
    {
        polygonPoints.push_back( ::ULIS::FVec2I( line->mStartPoint.X, line->mStartPoint.Y ) );
    }
    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GeneratePolygonPoints( polygonPoints, pointsArray );

    return pointsArray;
} */