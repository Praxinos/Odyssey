#include "OdysseyVectorChain.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"

FOdysseyVectorChain::~FOdysseyVectorChain()
{
}

FOdysseyVectorChain::FOdysseyVectorChain( FOdysseyVectorPath* iPath
                                        , FOdysseyVectorVertex* iUnchainedVertex )
    : mLength( 0.0f )
    , mBBox( 0.0f, 0.0f, 0.0f, 0.0f )
    , mPath( iPath )
{
    FOdysseyVectorVertex* currentVertex = iUnchainedVertex;
    FOdysseyVectorSegment* currentSegment = currentVertex->GetFirstSegment();

    mVertexArray.reserve( iPath->GetVertexList().size() );
    mSegmentArray.reserve( iPath->GetSegmentList().size() );

    mVertexArray.push_back( currentVertex );
    currentVertex->SetChained( true );

    while( currentSegment )
    {
        FOdysseyVectorVertex* nextVertex = currentSegment->GetOtherVertex( currentVertex );

        mSegmentArray.emplace_back( currentSegment );

        if( nextVertex->IsChained() == false )
        {
            FOdysseyVectorSegment* nextSegment = nextVertex->GetOtherSegment( currentSegment );

            mVertexArray.push_back( nextVertex );
            nextVertex->SetChained( true );

            currentVertex = nextVertex;
            currentSegment = nextSegment;
        }
        else
        {
            currentVertex = nextVertex;
            currentSegment = nullptr;
        }
    }
}

// callback must return false to keep iterating
void
FOdysseyVectorChain::Iterate( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSegment*)> iCallback )
{
    FOdysseyVectorVertex* currentVertex = mVertexArray[0];

    for( FOdysseyVectorSegment* segment : mSegmentArray )
    {
        FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( currentVertex );

        if( iCallback( currentVertex, segment ) == true )
        {
            return;
        }

        currentVertex = nextVertex;
    }
}

// returns current alpha value
uint8
FOdysseyVectorChain::GetAlpha( int32 iX, int32 iY, BLImageData* iImageData )
{
    if( ( x >= 0 ) && ( x < iImageData->size.w )
     && ( y >= 0 ) && ( y < iImageData->size.h ) )
    {
        uint8 *pixel = static_cast<uint8*>( iImageData->pixelData );
        uint32 offset = ( iY * iImageData->size.w ) + iX;

        return pixel[offset];
    }

    return 0;
}

// returns true if there were any intersection with the erasure zone
bool
FOdysseyVectorChain::CheckContrast( uint8 iAlphaValue0, uint8 iAlphaValue1 )
{
    if( ( iAlphaValue0 == 0 ) && iAlphaValue1 )
     || ( iAlphaValue0 && ( iAlphaValue1 == 0 ) )
    {
        return true;
    }

    return false;
}

// returns true if there were any intersection with the erasure zone
bool
FOdysseyVectorChain::TraceLine( uint8 iLastAlphaValue
                              , int32 iX0
                              , int32 iY0
                              , double iT0
                              , int32 iX1
                              , int32 iY1
                              , double iT1
                              , BLImageData* iImageData
                              , FOdysseyVectorSegment* iSegment
                              , std::vector<FWayPoint>& oWayPointArray )
{
    int32  dx  = ( iX1 - iX0 );
    uint32 ddx = abs ( dx );
    int32  dy  = ( iY1 - iY0 );
    uint32 ddy = abs ( dy );
    double dt  = ( iT1 - iT0 );
    int32  dd  = ( ddx > ddy ) ? ddx : ddy;
    int32  px  = ( dx > 0 ) ? 1 : -1;
    int32  py  = ( dy > 0 ) ? 1 : -1;
    double pt  = ( dd ) ? dt / dd : 0.0f;
    int32  x   = iX0;
    int32  y   = iY0;
    double t   = iT0;
    uint32 cumul = 0;
    uint8 lastAlphaValue = iLastAlphaValue;
    bool hasHit = false;

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            if( i == ddx ) t = iT1; // to address imprecision, we set the exact value on the last loop

            // we don't trace endpoints
            if( ( t != 0.0f ) && ( t != 1.0f ) )
            {
                uint8 alphaValue = GetAlpha( x, y, iImageData );

                if( CheckContrast( alphaValue, lastAlphaValue ) )
                {
                    oWayPointArray.emplace_back( nullptr
                                               , iSegment
                                               , t
                                               , ( alphaValue == 0 ) ? eWayPointType::LeavesErasureArea
                                                                     : eWayPointType::EntersErasureArea );

                    hasHit = true;
                }

                lastAlphaValue = alphaValue;
            }

            cumul += ddy;
            x     += px;
            t     += pt;

            if ( cumul >= ddx )
            {
                cumul -= ddx;
                y     += py;
            }
        }
    }
    else
    {
        for ( uint32 i = 0; i <= ddy; i++ )
        {
            if( i == ddy ) t = iT1; // to address imprecision, we set the exact value on the last loop

            // we don't trace endpoints
            if( ( t != 0.0f ) && ( t != 1.0f ) )
            {
                uint8 alphaValue = GetAlpha( x, y, iImageData );

                if( CheckContrast( alphaValue, lastAlphaValue ) )
                {
                    oWayPointArray.emplace_back( nullptr
                                               , iSegment
                                               , t
                                               , ( alphaValue == 0 ) ? eWayPointType::LeavesErasureArea
                                                                     : eWayPointType::EntersErasureArea );

                    hasHit = true;
                }

                lastAlphaValue = alphaValue;
            }

            cumul += ddx;
            y     += py;
            t     += pt;

            if ( cumul >= ddy )
            {
                cumul -= ddy;
                x     += px;
            }
        }
    }

    return hasHit;
}

bool
FOdysseyVectorChain::Trace( BLImageData* iImageData, std::vector<FWayPoint>& oWayPointarray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    ::ULIS::FVec2D& vertexCoords = mVertex->GetCoords();
    BLPoint vertexPoint = worldMatrix.mapPoint( vertexCoords.x, vertexCoords.y );
    uint8 vertexAlpha = GetAlpha( vertexPoint.x, vertexPoint.y, imageData );
    uint8 lastAlphaValue = vertexAlpha;
    bool hasHit = false;

    if( vertexAlpha == 0 ) // otherVertex in dark zone, keep it
    {
        oWayPointarray.emplace_back( otherVertex, segment, 0.0f );
    }

    Iterate( [ &worldMatrix
             , &oWayPointarray
             , &lastAlphaValue
             , &hasHit ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
             {
                 std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
                 FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex(vertex);
                 ::ULIS::FVec2D& otherVertexCoords = otherVertex->GetCoords();
                 BLPoint otherVertexPoint = worldMatrix.mapPoint( otherVertexCoords.x, otherVertexCoords.y );
                 uint8 otherVertexAlpha = GetAlpha( otherVertexPoint.x, otherVertexPoint.y, imageData );

                 if( vertex == segment->GetVertex(0) )
                 {
                     // iteration
                     for( auto it = fractionCache.begin(); it != fractionCache.end(); ++it )
                     {
                         FOdysseyVectorFraction& fraction = *it;

                         BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                         BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                         if( TraceLine( lastAlphaValue, p0.x, p0.y, p1.x, p1.y, fraction.fromT, fraction.toT, segment, iImageData, oWayPointarray ) )
                         {
                             hasHit = true;
                         }
                     }

                     if( otherVertexAlpha == 0 ) // otherVertex in dark zone, keep it
                     {
                         oWayPointarray.emplace_back( otherVertex, segment, 0.0f, eWayPointType::OutsideErasureArea );
                     }

                     lastAlphaValue = otherVertexAlpha;
                 }
                 else
                 {
                     // iteration for inverted segments
                     for( auto it = fractionCache.rbegin(); it != fractionCache.rend(); ++it )
                     {
                         FOdysseyVectorFraction& fraction = *it;

                         BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                         BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                         if( TraceLine( lastAlphaValue, p1.x, p1.y, p0.x, p0.y, fraction.toT, fraction.fromT, segment, iImageData, oWayPointarray ) )
                         {
                             hasHit = true;
                         }
                     }

                     if( otherVertexAlpha == 0 ) // otherVertex in dark zone, keep it
                     {
                         oWayPointarray.emplace_back( otherVertex, segment, 1.0f, eWayPointType::OutsideErasureArea );
                     }

                     lastAlphaValue = otherVertexAlpha;
                 }

                 return false;
             } );

    return hasHit;
}

uint32
FOdysseyVectorChain::HitMask( BLContext* iBLContext, std::vector<FOdysseyVectorObject*>& oNewPathArray )
{
    // the mask image must be selected by the vector engine at this point
    BLImage* blimg = mPath->GetEngine()->GetBLMask();

    if( blimg )
    {
        BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
        std::vector<FWayPoint> wayPointarray;
        BLImageData imageData;

        blimg->getData( &imageData );

        wayPointarray.reserve( 10 );

        if( Trace( &imageData, wayPointarray ) )
        {
            FOdysseyVectorPath* newPath = nullptr;
            FWayPoint* segmentWayPoint[2] = { &wayPointarray[0], nullptr };

            for( int i = 0; i < wayPointarray.size() - 1; i++ )
            {
                int n = i + 1;

                if( ( newPath == nullptr ) || ( wayPointarray[i].type == eWayPointType::LeavesErasureArea ) )
                {
                    newPath = new FOdysseyVectorPath( mPath->GetName() );
                }

                if( ( wayPointarray[i].type != eWayPointType::EntersErasureArea )
                 && ( wayPointarray[n].type != eWayPointType::LeavesErasureArea ) )
                {
                    
                }
            }
        }
    }
}
