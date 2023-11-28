#include "OdysseyVectorChain.h"
#include "OdysseyVector.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"

FWaySegment::FWaySegment( FOdysseyVectorSegment* iSegment
                        , std::vector<FWayPoint>& iWayPointArray
                        , uint32 iIndexWayPoint0
                        , uint32 iIndexWayPoint1 )
{
    double t0, t1;

    segment = iSegment;
    // work with indexes because the wayPoint array is gonna be resized, so
    // we can't work with pointers
    indexWayPoint0 = iIndexWayPoint0;
    indexWayPoint1 = iIndexWayPoint1;

    if( iWayPointArray[indexWayPoint0].flags & FWayPoint::Original )
    {
        t0 = iWayPointArray[indexWayPoint0].vertex->GetT( iSegment );
    }
    else
    {
        t0 = iWayPointArray[indexWayPoint0].t;
    }

    if( iWayPointArray[indexWayPoint1].flags & FWayPoint::Original )
    {
        t1 = iWayPointArray[indexWayPoint1].vertex->GetT( iSegment );
    }
    else
    {
        t1 = iWayPointArray[indexWayPoint1].t;
    }

    if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
        ::ULIS::FVec2D* cubicSegmentBezier = cubicSegment->GetBezier();

        if( t0 < t1 )
        {
            FOdysseyVector::BezierExtract( cubicSegmentBezier[0]
                                         , cubicSegmentBezier[1]
                                         , cubicSegmentBezier[2]
                                         , cubicSegmentBezier[3]
                                         , t0
                                         , t1
                                         , bezier[0]
                                         , bezier[1]
                                         , bezier[2]
                                         , bezier[3] );
        }
        else
        {
            FOdysseyVector::BezierExtract( cubicSegmentBezier[0]
                                         , cubicSegmentBezier[1]
                                         , cubicSegmentBezier[2]
                                         , cubicSegmentBezier[3]
                                         , t1
                                         , t0
                                         , bezier[3]
                                         , bezier[2]
                                         , bezier[1]
                                         , bezier[0] );
        }
    }
}

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

        mSegmentArray.push_back( currentSegment );

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
    FOdysseyVectorVertex* currentVertex = mVertexArray.front();

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
    if( ( iX >= 0 ) && ( iX < iImageData->size.w )
     && ( iY >= 0 ) && ( iY < iImageData->size.h ) )
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
    if( ( ( iAlphaValue0 == 0 ) && iAlphaValue1 )
     || ( iAlphaValue0 && ( iAlphaValue1 == 0 ) ) )
    {
        return true;
    }

    return false;
}

// returns the waypoint to chain a next waypoint with
FWayPoint*
FOdysseyVectorChain::TraceLine( int32 iX0
                              , int32 iY0
                              , double iT0
                              , int32 iX1
                              , int32 iY1
                              , double iT1
                              , BLImageData* iImageData
                              , FWayPoint* iChainedWayPoint
                              , std::vector<FWayPoint>& oWayPointArray
                              , std::vector<FWaySegment>& oWaySegmentArray
                              , FOdysseyVectorSegment* iSegment
                              , bool iRevert )
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
    uint32 hitCount = 0;
    uint8 lastAlphaValue = GetAlpha( iX0, iY0, iImageData );
    FWayPoint* startWayPoint = iRevert ? &oWayPointArray[iSegment->GetVertex(1)->GetID()]
                                       : &oWayPointArray[iSegment->GetVertex(0)->GetID()];
    FWayPoint* finalWayPoint = iRevert ? &oWayPointArray[iSegment->GetVertex(0)->GetID()]
                                       : &oWayPointArray[iSegment->GetVertex(1)->GetID()];
    double startRadius = startWayPoint->vertex->GetRadius();
    double finalRadius = finalWayPoint->vertex->GetRadius();
    ::ULIS::FVec2D bezier[4];

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            uint8 alphaValue = GetAlpha( x, y, iImageData );

            if( CheckContrast( alphaValue, lastAlphaValue ) )
            {
                double radius = ( t * finalRadius ) + ( ( 1.0f - t ) * startRadius );
                ::ULIS::FVec2D newVertexAt = iSegment->GetPointAt( t );
                uint32 wayPointCount = oWayPointArray.size();
                FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( newVertexAt.x
                                                                          , newVertexAt.y
                                                                          , radius );

                oWayPointArray.emplace_back( newVertex, ( alphaValue == 0 ) ? FWayPoint::LeavesErasureArea
                                                                            : FWayPoint::EntersErasureArea, t );

                newVertex->SetID( wayPointCount );
                // record waypoints ID because the array might grow (thus the pointer would change)
                oWaySegmentArray.emplace_back( iSegment, oWayPointArray, iChainedWayPoint->vertex->GetID(), newVertex->GetID() );
                // prepare the next iteration
                iChainedWayPoint = &oWayPointArray.back();

                hitCount++;
            }

            lastAlphaValue = alphaValue;

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
            uint8 alphaValue = GetAlpha( x, y, iImageData );

            if( CheckContrast( alphaValue, lastAlphaValue ) )
            {
                double radius = ( t * finalRadius ) + ( ( 1.0f - t ) * startRadius );
                ::ULIS::FVec2D newVertexAt = iSegment->GetPointAt( t );
                uint32 wayPointCount = oWayPointArray.size();
                FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( newVertexAt.x
                                                                          , newVertexAt.y
                                                                          , radius );

                oWayPointArray.emplace_back( newVertex, ( alphaValue == 0 ) ? FWayPoint::LeavesErasureArea
                                                                            : FWayPoint::EntersErasureArea, t );

                newVertex->SetID( wayPointCount );
                // record waypoints ID because the array might grow (thus the pointer would change)
                oWaySegmentArray.emplace_back( iSegment, oWayPointArray, iChainedWayPoint->vertex->GetID(), newVertex->GetID() );
                // prepare the next iteration
                iChainedWayPoint = &oWayPointArray.back();

                hitCount++;
            }

            lastAlphaValue = alphaValue;

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

    return iChainedWayPoint;
}

bool
FOdysseyVectorChain::Trace( BLImageData* iImageData
                          , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                          , std::vector<FWayPoint>& oWayPointArray
                          , std::vector<FWaySegment>& oWaySegmentArray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    FWayPoint* chainedWayPoint;
    bool hasHit = false;

    oWayPointArray.clear();
    // reserve 1 point per vertex + 2 point per segment
    oWayPointArray.reserve( mVertexArray.size() + ( mSegmentArray.size() * 2 ) );

    oWaySegmentArray.clear();
    // reserve 3 segment per segment
    oWaySegmentArray.reserve( mSegmentArray.size() * 3 );

    // prepare indexes and determine if original vertices should be kept
    for( int i = 0; i < mVertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = mVertexArray[i];
        ::ULIS::FVec2D& coords = vertex->GetCoords();
        BLPoint point = worldMatrix.mapPoint( coords.x, coords.y );
        uint8 alpha = GetAlpha( point.x, point.y, iImageData );

        if( alpha == 0 ) // vertex in dark zone, keep it
        {
            // Note: this is written twice if a vertex has 2 segments.
            // It does not matter because we use indexes.
            oWayPointArray.emplace_back( vertex
                                       , FWayPoint::OutsideErasureArea
                                       | FWayPoint::Original );
        }
        else
        {
            // Note: this is written twice if a vertex has 2 segments.
            // It does not matter because we use indexes.
            oWayPointArray.emplace_back( vertex
                                       , FWayPoint::InsideErasureArea
                                       | FWayPoint::Original );

            hasHit = true;
        }

        vertex->SetID( i );
    }

    chainedWayPoint = &oWayPointArray[0];

    Iterate( [ this
             , iImageData
             , &chainedWayPoint
             , &oWayPointArray
             , &oWaySegmentArray
             , &oRemovedVertexArray
             , &oRemovedSegmentArray 
             , &hasHit
             , &worldMatrix ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
               {
                   std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
                   FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
                   FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
                   bool revert = ( vertex == segment->GetVertex(0) ) ? false : true;
                   uint32 hitCount = 0;

                   // iteration
                   if( revert == false )
                   {
                       for( auto it = fractionCache.begin(); it != fractionCache.end(); ++it )
                       {
                           FOdysseyVectorFraction& fraction = *it;

                           BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                           BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                           chainedWayPoint = TraceLine( p0.x
                                                      , p0.y
                                                      , fraction.fromT
                                                      , p1.x
                                                      , p1.y
                                                      , fraction.toT
                                                      , iImageData
                                                      , chainedWayPoint
                                                      , oWayPointArray
                                                      , oWaySegmentArray
                                                      , segment
                                                      , revert );
                       }
                       // don't forget the last sub-segment
                       oWaySegmentArray.emplace_back( segment
                                                    , oWayPointArray
                                                    , chainedWayPoint->vertex->GetID()
                                                    , vertex1->GetID() );

                       chainedWayPoint = &oWayPointArray[vertex1->GetID()];
                   }
                   else
                   {
                       for( auto it = fractionCache.rbegin(); it != fractionCache.rend(); ++it )
                       {
                           FOdysseyVectorFraction& fraction = *it;

                           BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                           BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                           chainedWayPoint = TraceLine( p1.x
                                                      , p1.y
                                                      , fraction.toT
                                                      , p0.x
                                                      , p0.y
                                                      , fraction.fromT
                                                      , iImageData
                                                      , chainedWayPoint
                                                      , oWayPointArray
                                                      , oWaySegmentArray
                                                      , segment
                                                      , revert );
                       }
                       // don't forget the last sub-segment
                       oWaySegmentArray.emplace_back( segment
                                                    , oWayPointArray
                                                    , chainedWayPoint->vertex->GetID()
                                                    , vertex0->GetID() );

                       chainedWayPoint = &oWayPointArray[vertex0->GetID()];
                   }

                    //---------------- parse hits if any ---------------------//
                   if( oWayPointArray.size() > mVertexArray.size() )
                   {
                       hasHit = true;
                   }

                   return false; // keep iterating;
               } );

    return hasHit;
}

bool
FOdysseyVectorChain::HitMask( BLImageData* iImageData
                            , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                            , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                            , std::vector<FWayPoint>& oWayPointArray
                            , std::vector<FWaySegment>& oWaySegmentArray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();

    return Trace( iImageData
                , oRemovedVertexArray
                , oRemovedSegmentArray
                , oWayPointArray
                , oWaySegmentArray );

    return false;
}
