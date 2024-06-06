#include "InbetweenerTag/InbetweenerQuad.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerQuad::FInbetweenerQuad( )
    : mFlags ( 0 )
    , bPinned( false )
{
}

FInbetweenerPoint** 
FInbetweenerQuad::GetPoints()
{
    return mPoint;
}

bool
FInbetweenerQuad::IsLinked()
{
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
    ComputeCentroid(eInbetweenerPointPositionType::MotionPosition);
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
        mCentroid[eInbetweenerPointPositionType::MotionPosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::MotionPosition );
        mCentroid[eInbetweenerPointPositionType::DeformPosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::DeformPosition );
        mCentroid[eInbetweenerPointPositionType::TargetPosition] += mPoint[j]->GetPosition( eInbetweenerPointPositionType::TargetPosition );
    }

    for ( uint32 i = 0; i < 4; ++i )
    {
        mCentroid[i] *= 0.25f;
    }
*/
}
