// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

#define FRACTIONCOUNT 24

FInbetweenerChart::HUDBezier::HUDBezier( FInbetweenerChart* iChart )
    : mChart ( iChart )
    , mPoint { this, this, this }
{
    mFractionBuffer.resize( FRACTIONCOUNT );
}

bool
FInbetweenerChart::HUDBezier::IsInvalidated()
{
    return bInvalidated;
}

void
FInbetweenerChart::HUDBezier::Update()
{
    ::ULIS::FVec2D quadratic[3] = { mPoint[0].GetPosition()
                                  , mPoint[1].GetPosition()
                                  , mPoint[2].GetPosition() };
/*
    if( bLink )
    {
        FInbetweenerBreakdown* prevBreakdown = mChart->GetBreakdown()->GetPrevBreakdown();

        if( prevBreakdown )
        {
            quadratic[0] = prevBreakdown->GetChart()->GetFullHUDBezier()->GetPoints()[2].GetPosition();
        }
    }
*/
    std::vector<double> fractionLengthBuffer;
    double quadraticT0 = 0.0f;
    double linearT0 = 0.0f;
    double stepT = 1.0f / FRACTIONCOUNT;
    ::ULIS::FVec2D p0;
    double totalFractionLength = 0.0f;

    double quadraticBezierLength = FOdysseyVector::GetQuadraticBezierApproximateLength( quadratic
                                                                                      , FRACTIONCOUNT
                                                                                      , &fractionLengthBuffer
                                                                                      , &mFractionPointBuffer );

    // build a lookup table for getting linear values for t
    for( uint32 i = 0; i < FRACTIONCOUNT; i++ )
    {
        double quadraticT1 = quadraticT0 + stepT;
        double linearT1 = linearT0 + ( fractionLengthBuffer[i] / quadraticBezierLength );

        totalFractionLength += fractionLengthBuffer[i];

        mFractionBuffer[i].linearT0 = linearT0;
        mFractionBuffer[i].linearT1 = linearT1;
        mFractionBuffer[i].quadraticT0  = quadraticT0;
        mFractionBuffer[i].quadraticT1  = quadraticT1;

        quadraticT0 = quadraticT1;
        linearT0 = linearT1;
    }
    mFractionBuffer.back().linearT1 = 1.0f;
    mFractionBuffer.back().quadraticT1 = 1.0f;

    bInvalidated = false;
}

double
FInbetweenerChart::HUDBezier::HitTest( const ::ULIS::FVec2D& iPt, uint32 iRadius )
{
    float quadraticT = 0.0f;
    double minDistance = DBL_MAX;
    FInbetweenerChart::HUDBezier::Fraction* closestFraction = nullptr;
    double closestFractionT = 0.0f;

    for( uint32 i = 0; i < FRACTIONCOUNT; i++ )
    {
        uint32 n = i + 1;
        ::ULIS::FVec2D* segPoint0 = &mFractionPointBuffer[i];
        ::ULIS::FVec2D* segPoint1 = &mFractionPointBuffer[n];
        double distanceToSegment;
        double t = FOdysseyVector::DistanceToSegmentConstrained( iPt
                                                               , *segPoint0
                                                               , *segPoint1
                                                               , distanceToSegment );

        if( distanceToSegment < minDistance )
        {
            minDistance = distanceToSegment;

            closestFraction = &mFractionBuffer[i];
            closestFractionT = closestFraction->linearT0 + ( ( closestFraction->linearT1
                                                             - closestFraction->linearT0 ) * t );
        }
    }

    if( minDistance <= iRadius )
    {
        return closestFractionT;
    }

    return -1.0f;
}

double
FInbetweenerChart::HUDBezier::GetQuadraticT( float iSpacingT )
{
    float quadraticT = 0.0f;

    for( uint32 i = 0; i < FRACTIONCOUNT; i++ )
    {
        FInbetweenerChart::HUDBezier::Fraction* fraction = &mFractionBuffer[i];

        if( ( iSpacingT >= fraction->linearT0 ) && ( iSpacingT <= fraction->linearT1 ) )
        {
            float diffLinear = fraction->linearT1 - fraction->linearT0;

            if( diffLinear )
            {
                float diffCubic = fraction->quadraticT1 - fraction->quadraticT0;
                float ratio = ( iSpacingT - fraction->linearT0 ) / diffLinear;

                quadraticT = fraction->quadraticT0 + ( diffCubic * ratio );

                break;
            }
        }
    }

    return quadraticT;
}

void
FInbetweenerChart::HUDBezier::Invalidate()
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = mChart->GetBreakdown()->GetInbetweenerTag();

    bInvalidated = true;

    inbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CHARTHUD) );
}

FInbetweenerChart::HUDBezier::Point*
FInbetweenerChart::HUDBezier::GetPoints()
{
    return mPoint;
}

FInbetweenerChart*
FInbetweenerChart::HUDBezier::GetChart()
{
    return mChart;
}

//////////////////////////////////////////////////////////////////////////////


FInbetweenerChart::HUDBezier::Point::Point( FInbetweenerChart::HUDBezier* iHUDBezier )
    : mHUDBezier ( iHUDBezier )
{

}

FInbetweenerChart::HUDBezier*
FInbetweenerChart::HUDBezier::Point::GetHUDBezier()
{
    return mHUDBezier;
}

void
FInbetweenerChart::HUDBezier::Point::SetPosition( double iX, double iY )
{
    mPosition.x = iX;
    mPosition.y = iY;

    if( this == &mHUDBezier->GetPoints()[0] )
    {
        FInbetweenerBreakdown* prevBreakdown = mHUDBezier->GetChart()->GetBreakdown()->GetPrevBreakdown();

        if( prevBreakdown )
        {
            // set coords directly or else expect an infinite loop
            prevBreakdown->GetChart()->GetHUDBezier()->GetPoints()[2].mPosition = ::ULIS::FVec2D( iX, iY );

            prevBreakdown->GetChart()->GetHUDBezier()->Invalidate();
        }
    }

    if( this == &mHUDBezier->GetPoints()[2] )
    {
        FInbetweenerBreakdown* nextBreakdown = mHUDBezier->GetChart()->GetBreakdown()->GetNextBreakdown();

        if( nextBreakdown )
        {
            // set coords directly or else expect an infinite loop
            nextBreakdown->GetChart()->GetHUDBezier()->GetPoints()[0].mPosition = ::ULIS::FVec2D( iX, iY );

            nextBreakdown->GetChart()->GetHUDBezier()->Invalidate();
        }
    }

    mHUDBezier->Invalidate();
}

::ULIS::FVec2D
FInbetweenerChart::HUDBezier::Point::GetPosition()
{
    return mPosition;
}


//////////////////////////////////////////////////////////////////////////////


std::vector<FInbetweenerChart::HUDBezier::Fraction>&
FInbetweenerChart::HUDBezier::GetFractionBuffer()
{
    return mFractionBuffer;
}

std::vector<::ULIS::FVec2D>&
FInbetweenerChart::HUDBezier::GetFractionPointBuffer()
{
    return mFractionPointBuffer;
}


//////////////////////////////////////////////////////////////////////////////


FInbetweenerChart::Inbetween::Inbetween( FInbetweenerChart* iChart )
    : mChart( iChart )
    , mSpacing( 0.0f )
    , mDrawing ( nullptr )
{

}

uint32
FInbetweenerChart::Inbetween::GetIndex()
{
    return this - &mChart->GetInbetweenBuffer()[0];
}

uint32
FInbetweenerChart::Inbetween::GetIndexInInbetweener()
{
    return GetIndex() + mChart->GetBreakdown()->GetSourceDrawingIndex();
}

int32
FInbetweenerChart::Inbetween::GetCellIndex()
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = mChart->GetBreakdown()->GetInbetweenerTag();
    uint32 tagCellIndex = inbetweenerTag->GetSourceCellIndex();
    uint32 inbetweenIndex = mChart->GetBreakdown()->GetSourceDrawingIndex() + GetIndex();

    return (int32)tagCellIndex + (int32)( inbetweenIndex * (int)inbetweenerTag->GetInterpolationDirection());
}

void
FInbetweenerChart::Inbetween::SetSpacing( float iSpacing )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = mChart->GetBreakdown()->GetInbetweenerTag();

    mSpacing = iSpacing;

    inbetweenerTag->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING) );
}

float
FInbetweenerChart::Inbetween::GetSpacing()
{
    return mSpacing;
}

void
FInbetweenerChart::Inbetween::SetDrawing( FInbetweenerDrawing* iDrawing )
{
    mDrawing = iDrawing;
}

FInbetweenerDrawing*
FInbetweenerChart::Inbetween::GetDrawing( )
{
    return mDrawing;
}

FInbetweenerChart*
FInbetweenerChart::Inbetween::GetChart( )
{
    return mChart;
}

FInbetweenerChart::~FInbetweenerChart()
{
}

/*
FInbetweenerChart::FInbetweenerChart()
    : mBreakdown( nullptr  )
{
    mDivisionBuffer.reserve( 16 );
}
*/
FInbetweenerChart::FInbetweenerChart( FInbetweenerBreakdown* iBreakdown )
    : mBreakdown( iBreakdown )
    , mHUDBezier ( this  )
{
    mHUDBezier.GetPoints()[0].SetPosition( DEFAULT_POSITION_P0_X, DEFAULT_POSITION_P0_Y );
    mHUDBezier.GetPoints()[1].SetPosition( DEFAULT_POSITION_P1_X, DEFAULT_POSITION_P1_Y );
    mHUDBezier.GetPoints()[2].SetPosition( DEFAULT_POSITION_P2_X, DEFAULT_POSITION_P2_Y );

    mInbetweenBuffer.reserve( 16 );

    mInbetweenBuffer.emplace_back( this ).SetSpacing( 0.0f );
    mInbetweenBuffer.emplace_back( this ).SetSpacing( 1.0f );
}

FInbetweenerBreakdown*
FInbetweenerChart::GetBreakdown()
{
    return mBreakdown;
}

std::vector<FInbetweenerChart::Inbetween>&
FInbetweenerChart::GetInbetweenBuffer()
{
    return mInbetweenBuffer;
}

void
FInbetweenerChart::GetSpacing( std::vector<float>& oSpacingArray )
{
    oSpacingArray.clear();
    oSpacingArray.reserve( mInbetweenBuffer.size() );

    for( FInbetweenerChart::Inbetween& inbetween : mInbetweenBuffer )
    {
        oSpacingArray.push_back( inbetween.GetSpacing() );
    }
}

void
FInbetweenerChart::Reset( bool iResetPositionning )
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    float stepT = 1.0f / ( drawingCount - 1 );
    float nextT = 0.0f;

    for( uint32 i = 0; i < drawingCount - 1; i++ )
    {
        mInbetweenBuffer[i].SetSpacing ( nextT );

        nextT += stepT;
    }
    // due to float imprecision, we get sure the last one is 1.0f
    mInbetweenBuffer.back().SetSpacing ( 1.0f );

    if( iResetPositionning )
    {
        mHUDBezier.GetPoints()[0].SetPosition( DEFAULT_POSITION_P0_X
                                             , DEFAULT_POSITION_P0_Y );
        mHUDBezier.GetPoints()[1].SetPosition( DEFAULT_POSITION_P1_X
                                             , DEFAULT_POSITION_P1_Y );
        mHUDBezier.GetPoints()[2].SetPosition( DEFAULT_POSITION_P2_X
                                             , DEFAULT_POSITION_P2_Y );
    }

    mBreakdown->GetInbetweenerTag()->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING )
                                                                                                    .Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CELLS ) );
}

void
FInbetweenerChart::Resize()
{
    uint32 drawingCount = mBreakdown->GetDrawingCount();
    FInbetweenerChart::Inbetween& lastInbetween = *std::prev( mInbetweenBuffer.end(), 2 );
    uint32 fromIndex = lastInbetween.GetIndex();
    float fromT = lastInbetween.GetSpacing();
    float stepT = (  1.0f - fromT ) / ( drawingCount - fromIndex - 1 );
    float nextT = fromT;
    std::vector<float> spacingArray;

    // remember former spacing
    GetSpacing( spacingArray );

    mInbetweenBuffer.resize( drawingCount, this );

    if( drawingCount )
    {
        for( uint32 i = fromIndex; i < drawingCount - 1; i++ )
        {
            mInbetweenBuffer[i].SetSpacing ( nextT );

            nextT += stepT;
        }
    }
    else
    {
        mInbetweenBuffer.front().SetSpacing ( 0.0f );
    }

    mInbetweenBuffer.back().SetSpacing ( 1.0f );

    mBreakdown->GetInbetweenerTag()->GetOwner()->Invalidate( FOdysseyVectorObjectInvalidationFlags().Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SPACING )
                                                                                                    .Set( FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_CELLS ) );
}

FInbetweenerChart::HUDBezier*
FInbetweenerChart::GetHUDBezier()
{
    return &mHUDBezier;
}
