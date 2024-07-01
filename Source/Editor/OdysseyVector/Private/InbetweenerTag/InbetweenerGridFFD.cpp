#include "InbetweenerTag/InbetweenerGridFFD.h"
#include "InbetweenerTag/InterpolatedPath.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

FInbetweenerGridFFD::FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                        , uint32 iNumQuadX
                                        , uint32 iNumQuadY )
    : FInbetweenerGrid( iInbetweenerTag, iNumQuadX, iNumQuadY )
{
}

::ULIS::FVec2D
FInbetweenerGridFFD::DeformPoint( FInterpolatedPoint* iInterpolatedPoint )
{
    double interpolatedPointU = iInterpolatedPoint->GetU();
    double interpolatedPointV = iInterpolatedPoint->GetV();
    ::ULIS::FRectD bbox = mInbetweenerTag->GetSourceBBox( false );
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
            double gridPointU = mPointBuffer[offset].GetU();
            double gridPointV = mPointBuffer[offset].GetV();

            vj.x += ( bcu * pow ( ( 1 - interpolatedPointU ), (mNumQuadX) - j ) * pow ( interpolatedPointU, j ) * gridPointU );
            vj.y += ( bcu * pow ( ( 1 - interpolatedPointU ), (mNumQuadX) - j ) * pow ( interpolatedPointU, j ) * gridPointV );
        }

        vi.x += ( bcv * pow ( ( 1 - interpolatedPointV ), (mNumQuadY) - i ) * pow ( interpolatedPointV, i ) * vj.x );
        vi.y += ( bcv * pow ( ( 1 - interpolatedPointV ), (mNumQuadY) - i ) * pow ( interpolatedPointV, i ) * vj.y );
    }

    return ::ULIS::FVec2D( ( bbox.x + ( bbox.w * vi.x ) )
                         , ( bbox.y + ( bbox.h * vi.y ) ) );
}

void
FInbetweenerGridFFD::Make( uint32 iNumQuadX
                         , uint32 iNumQuadY
                         , const ::ULIS::FRectD& iBBox
                         , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                         , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer )
{
    FInbetweenerGrid::Make( iNumQuadX
                          , iNumQuadY
                          , iBBox
                          , iSourcePositionBuffer
                          , iTargetPositionBuffer );

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

void
FInbetweenerGridFFD::MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer
                                         , const BLMatrix2D& iSpaceInverseMatrix )
{
    ::ULIS::FRectD spaceBBox = mInbetweenerTag->GetSourceBBox( false );
    BLMatrix2D conversionMatrix;
    uint32 pointID = 0;
    uint32 segmentID = 0;

    mUsedQuadCount = 0;
    mUsedPointCount = 0;

    // reset point status
    for( FInbetweenerPoint& point : mPointBuffer )
    {
        point.SetNeeded( false );
    }

    // relink unlinked quads before discarding unused ones at the end of the function
    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() == false )
        {
            quad.Link();
        }
    }

    for( FInterpolatedPath& interpolatedPath : iPathBuffer )
    {
        std::vector<FInterpolatedPoint>& interpolatedPointBuffer = interpolatedPath.GetInterpolatedPointBuffer();

        FOdysseyVector::MatrixMultiply( iSpaceInverseMatrix
                                      , interpolatedPath.GetOriginalPath()->GetWorldMatrix()
                                      , conversionMatrix );

        for( FInterpolatedPoint& interpolatedPoint : interpolatedPointBuffer )
        {
            FOdysseyVectorPoint* originalPoint = interpolatedPoint.GetOriginalPoint();
            BLPoint pt = conversionMatrix.mapPoint( originalPoint->GetX()
                                                  , originalPoint->GetY() );
            double spaceX = pt.x - spaceBBox.x;
            double spaceY = pt.y - spaceBBox.y;
            //double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
            //double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );
            double u = std::clamp<double>( spaceX / spaceBBox.w, 0.0f, 1.0f );
            double v = std::clamp<double>( spaceY / spaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
            interpolatedPoint.SetUV( nullptr, u, v );
        }
    }

    mQuadArray.clear();
    mQuadArray.reserve( mQuadBuffer.size() );

    // TODO: do this in base class
    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            mUsedQuadCount++;
            mQuadArray.push_back( &quad );
        }
    }

    for( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            point.SetID( mUsedPointCount++ );
        }
    }
    //---------------
}
