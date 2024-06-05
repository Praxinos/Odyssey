#include "InbetweenerTag/InbetweenerGridFFD.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

FInbetweenerGridFFD::FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                        , uint32 iNumQuadX
                                        , uint32 iNumQuadY )
    : FInbetweenerGrid( iInbetweenerTag, iNumQuadX, iNumQuadY )
{
}

::ULIS::FVec2D
FInbetweenerGridFFD::DeformPoint( FInterpolatedPoint* iInterpolatedPoint )
{
    ::ULIS::FRectD bbox = mInbetweenerTag->GetOwner()->GetBBox( false );
    ::ULIS::FVec2D vi = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 numVertexX = mNumQuadX + 1;
    uint32 numVertexY = mNumQuadY + 1;

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        ::ULIS::FVec2D vj = ::ULIS::FVec2D( 0.0f, 0.0f );
        double bcv = mVBinomialCoefficientBuffer[i];

        for ( uint32 j = 0; j < numVertexX; j++ )
        {
            double bcu = mUBinomialCoefficientBuffer[j];
            uint32 offset = ( i * numVertexX ) + j;

            vj.x += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumQuadX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mPointBuffer[offset].u );
            vj.y += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumQuadX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mPointBuffer[offset].v );
        }

        vi.x += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumQuadY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.x );
        vi.y += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumQuadY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.y );
    }

    return ::ULIS::FVec2D( ( bbox.x + ( bbox.w * vi.x ) )
                         , ( bbox.y + ( bbox.h * vi.y ) ) );
}

void
FInbetweenerGridFFD::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex )
{

    for( FInterpolatedPath& interpolatedPath : iInterpolatedPathBuffer )
    {
        std::vector<::ULIS::FVec2D>& interpolatedPointPositionBuffer = interpolatedPath.GetInterpolatedPointPositionBuffer();
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        uint32 skippedOffset = ( iInbetweenIndex * pointCount );

        for( uint32 i = 0; i < pointCount; i++ )
        {
            FInterpolatedPoint* interpolatedPoint = &interpolatedPath.GetInterpolatedPointBuffer()[i];

            interpolatedPointPositionBuffer[skippedOffset + i] = DeformPoint( interpolatedPoint );
        }
    }
}

void
FInbetweenerGridFFD::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    FInbetweenerGrid::Make( iNumQuadX, iNumQuadY );

    if( iNumQuadX && iNumQuadY )
    {
        mUBinomialCoefficientBuffer.resize( mPointBuffer.size() );
        mVBinomialCoefficientBuffer.resize( mPointBuffer.size() );

        ComputeBinomialCoefficients();
    }
}

static
int Factorial ( int n )
{
    return ( n <= 1 ) ? 1 : n * Factorial ( n - 1 ); 
}

static
double BinomialCoeff ( int n, int k )
{
    int div = Factorial ( k ) * Factorial ( n - k );

    if ( div == 0 ) return 0.0f;

    return ( double ) Factorial ( n ) / div;
}

// precompute binaomial coefficient
void
FInbetweenerGridFFD::ComputeBinomialCoefficients()
{
    uint32 numVertexX = mNumQuadX + 1;
    uint32 numVertexY = mNumQuadY + 1;

    for ( uint32 i = 0; i < numVertexX; i++ )
    {
        double coeffU = BinomialCoeff ( mNumQuadX, i );

        mUBinomialCoefficientBuffer[i] = coeffU;
    }

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        double coeffV = BinomialCoeff ( mNumQuadY, i );

        mVBinomialCoefficientBuffer[i] = coeffV;
    }
}
