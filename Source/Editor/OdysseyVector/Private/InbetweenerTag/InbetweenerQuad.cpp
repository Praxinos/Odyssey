#include "InbetweenerTag/InbetweenerQuad.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

FInbetweenerQuad::FInbetweenerQuad()
    : mFlags ( 0 )
    , bPinned( false )
{
}

FInbetweenerPoint** 
FInbetweenerQuad::GetPoints()
{
    return mPoint;
}

FInbetweenerGrid*
FInbetweenerQuad::GetGrid()
{
    return mGrid;
}

void
FInbetweenerQuad::Init( FInbetweenerGrid* iGrid )
{
    mGrid = iGrid;
}

bool
FInbetweenerQuad::IsLinked()
{
    FInbetweenerBreakdown* masterBreakdown = mGrid->GetBreakdown()->GetMasterBreakdown();

    if( masterBreakdown )
    {
        uint32 quadIndex = this - &mGrid->GetQuadBuffer()[0];

        return masterBreakdown->GetGrid()->GetQuadBuffer()[quadIndex].IsLinked();
    }

    return ( mFlags & LINKED ) ? true : false; 
}

void
FInbetweenerQuad::Link()
{
    mFlags |= LINKED;

    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->AddQuad( this );
    }
}

bool
FInbetweenerQuad::HitTest( double iLocalX, double iLocalY )
{
    ::ULIS::FRectD rect = ::ULIS::FRectD( mPoint[0]->GetSourcePosition().x
                                        , mPoint[0]->GetSourcePosition().y
                                        , mPoint[1]->GetSourcePosition().x - mPoint[0]->GetSourcePosition().x
                                        , mPoint[3]->GetSourcePosition().y - mPoint[0]->GetSourcePosition().y );

    return rect.HitTest( ::ULIS::FVec2D( iLocalX, iLocalY ) );
}

bool
FInbetweenerQuad::IsPinned()
{
    return bPinned;
}

double
FInbetweenerQuad::GetPinU()
{
    return mPinU;
}

double
FInbetweenerQuad::GetPinV()
{
    return mPinV;
}

::ULIS::FRectD
FInbetweenerQuad::GetBBox( eInbetweenerPointPositionType iPositionType )
{
    ::ULIS::FVec2D p0Position = mPoint[0]->GetPosition( iPositionType );
    ::ULIS::FVec2D p1Position = mPoint[1]->GetPosition( iPositionType );
    ::ULIS::FVec2D p2Position = mPoint[2]->GetPosition( iPositionType );
    ::ULIS::FVec2D p3Position = mPoint[3]->GetPosition( iPositionType );

    double xmin = ::ULIS::FMath::Min4( p0Position.x, p1Position.x, p2Position.x, p3Position.x );
    double xmax = ::ULIS::FMath::Max4( p0Position.x, p1Position.x, p2Position.x, p3Position.x );
    double ymin = ::ULIS::FMath::Min4( p0Position.y, p1Position.y, p2Position.y, p3Position.y );
    double ymax = ::ULIS::FMath::Max4( p0Position.y, p1Position.y, p2Position.y, p3Position.y );

    return ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
}

::ULIS::FVec2D
FInbetweenerQuad::GetPinPosition()
{
    return mPinPosition;
}

void
FInbetweenerQuad::Unlink()
{
    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->RemoveQuad( this );
    }

    mFlags &= (~LINKED);
}

::ULIS::FVec2D
FInbetweenerQuad::GetPoint( eInbetweenerPointPositionType iPositionType
                          , double iU
                          , double iV )
{
    return ( mPoint[0]->GetPosition( iPositionType ) * ( 1.0f - iU ) + mPoint[1]->GetPosition(iPositionType) * iU ) * ( 1.0 - iV ) // top edge
         + ( mPoint[3]->GetPosition( iPositionType ) * ( 1.0f - iU ) + mPoint[2]->GetPosition(iPositionType) * iU ) * iV;  // bottom edge
}

::ULIS::FVec2D
FInbetweenerQuad::BiasedCentroid( eInbetweenerPointPositionType iPositionType )
{
    if ( bPinned )
    {
        ::ULIS::FVec2D biasedCentroid = ::ULIS::FVec2D( 0.0f, 0.0f );
        double weight = 10000.0f;

        for ( uint32 i = 0; i < 4; ++i )
        {
            biasedCentroid += mPoint[i]->GetPosition( iPositionType );
        }

        if ( iPositionType != eInbetweenerPointPositionType::TargetPosition )
        {
            biasedCentroid += GetPoint( iPositionType, mPinU, mPinV ) * weight;
        }
        else
        {
            biasedCentroid += mPinPosition * weight;
        }

        biasedCentroid /= ( 4 + weight );

        return biasedCentroid;
    }

    return mCentroid[iPositionType];
}

double
FInbetweenerQuad::GetSourceArea()
{
    ::ULIS::FVec2D p0SourcePos = mPoint[0]->GetSourcePosition();
    ::ULIS::FVec2D p1SourcePos = mPoint[1]->GetSourcePosition();
    ::ULIS::FVec2D p2SourcePos = mPoint[2]->GetSourcePosition();
    ::ULIS::FVec2D p3SourcePos = mPoint[3]->GetSourcePosition();

    return FOdysseyVector::Cross2D( ( p1SourcePos - p0SourcePos )
                                  , ( p2SourcePos - p1SourcePos ) );

}

void
FInbetweenerQuad::ComputeCentroid( eInbetweenerPointPositionType iPositionType )
{
    mCentroid[iPositionType] = ::ULIS::FVec2D( 0.0f, 0.0f );

    for ( uint32 i = 0; i < 4; ++i )
    {
        mCentroid[iPositionType] += mPoint[i]->GetPosition( iPositionType );
    }

    mCentroid[iPositionType] *= 0.25f;
}

void
FInbetweenerQuad::ComputeCentroids()
{
    ComputeCentroid(eInbetweenerPointPositionType::SourcePosition);
    ComputeCentroid(eInbetweenerPointPositionType::InterpPosition);
    ComputeCentroid(eInbetweenerPointPositionType::DeformPosition);
    ComputeCentroid(eInbetweenerPointPositionType::TargetPosition);
/*
    for ( uint32 i = 0; i < 4; ++i)
    {
        mCentroid[i] = ::ULIS::FVec2D( 0.0f, 0.0f );
    }

    for ( uint32 j = 0; j < 4; j++ )
    {
        mCentroid[eInbetweenerPointPositionType::SourcePosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::SourcePosition );
        mCentroid[eInbetweenerPointPositionType::InterpPosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::InterpPosition );
        mCentroid[eInbetweenerPointPositionType::DeformPosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::DeformPosition );
        mCentroid[eInbetweenerPointPositionType::TargetPosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::TargetPosition );
    }

    for ( uint32 i = 0; i < 4; ++i )
    {
        mCentroid[i] *= 0.25f;
    }
*/
}
