#include "OdysseyVectorChain.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"

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

// returns the number of times there were any intersection with the erasure zone
uint32
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
    uint32 hitCount = 0;

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            uint8 alphaValue = GetAlpha( x, y, iImageData );

            if( CheckContrast( alphaValue, lastAlphaValue ) )
            {
                oWayPointArray.emplace_back( nullptr
                                           , iSegment
                                           , t
                                           , ( alphaValue == 0 ) ? eWayPointType::LeavesErasureArea
                                                                 : eWayPointType::EntersErasureArea );

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
                oWayPointArray.emplace_back( nullptr
                                           , iSegment
                                           , t
                                           , ( alphaValue == 0 ) ? eWayPointType::LeavesErasureArea
                                                                 : eWayPointType::EntersErasureArea );

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

    return hitCount;
}

bool
FOdysseyVectorChain::Trace( BLImageData* iImageData
                          , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                          , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                          , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    //std::list<FOdysseyVectorVertex*> newVertexList;
    //std::list<FOdysseyVectorSegment*> newSegmentList;
    bool hasHit = false;

    Iterate( [ this
             , iImageData
             , &oAddedVertexArray
             , &oAddedSegmentArray
             , &oRemovedVertexArray
             , &oRemovedSegmentArray 
             , &hasHit
             , &worldMatrix ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
               {
                   std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
                   FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
                   FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
                   ::ULIS::FVec2D& coords0 = vertex0->GetCoords();
                   ::ULIS::FVec2D& coords1 = vertex1->GetCoords();
                   BLPoint point0 = worldMatrix.mapPoint( coords0.x, coords0.y );
                   BLPoint point1 = worldMatrix.mapPoint( coords1.x, coords1.y );
                   uint8 alpha0 = GetAlpha( point0.x, point0.y, iImageData );
                   uint8 alpha1 = GetAlpha( point1.x, point1.y, iImageData );
                   std::vector<FWayPoint> wayPointArray;
                   uint32 hitCount = 0;

                   wayPointArray.reserve( 10 );

                   if( alpha0 == 0 ) // vertex in dark zone, keep it
                   {
                       wayPointArray.emplace_back( vertex0, segment, 0.0f, eWayPointType::OutsideErasureArea );
                   }
                   else
                   {
                       // add vertex0 to the array of removed vertices only if it was not already added to that array
                       if( std::find( oRemovedVertexArray.begin(), oRemovedVertexArray.end(), vertex0 ) == oRemovedVertexArray.end() )
                       {
                           oRemovedVertexArray.push_back( vertex0 );
                       }

                       hitCount++;
                   }

                   // iteration
                   for( auto it = fractionCache.begin(); it != fractionCache.end(); ++it )
                   {
                       FOdysseyVectorFraction& fraction = *it;

                       BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                       BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                       hitCount += TraceLine( alpha0
                                            , p0.x
                                            , p0.y
                                            , p1.x
                                            , p1.y
                                            , fraction.fromT
                                            , fraction.toT
                                            , segment
                                            , iImageData
                                            , wayPointArray );
                   }

                   if( alpha1 == 0 ) // otherVertex in dark zone, keep it
                   {
                       wayPointArray.emplace_back( vertex1, segment, 1.0f, eWayPointType::OutsideErasureArea );
                   }
                   else
                   {
                       // add vertex1 to the array of removed vertices only if it was not already added to that array
                       if( std::find( oRemovedVertexArray.begin(), oRemovedVertexArray.end(), vertex1 ) == oRemovedVertexArray.end() )
                       {
                           oRemovedVertexArray.push_back( vertex1 );
                       }

                       hitCount++;
                   }

                    //---------------- parse hits ---------------------//
                   if( hitCount )
                   {
                       hasHit = true;

                       oRemovedSegmentArray.push_back( segment );

                       //mPath->RemoveSegment( segment );

                       // create new vertices
                       for( FWayPoint& wayPoint : wayPointArray )
                       {
                           if( wayPoint.vertex == nullptr )
                           {
                               ::ULIS::FVec2D newVertexAt = segment->GetPointAt( wayPoint.t );
                               FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( mPath
                                                                                         , newVertexAt.x
                                                                                         , newVertexAt.y
                                                                                         , 1.0f );

                               wayPoint.vertex = newVertex;

                               oAddedVertexArray.push_back( newVertex );

                               //mPath->AddVertex( newVertex );
                           }
                       }

                       for( int i = 0; i < wayPointArray.size() - 1; i++ )
                       {
                           int n = i + 1;

                           // both vertices cannot be outside the erasure area
                           if( ( wayPointArray[i].type != eWayPointType::OutsideErasureArea )
                            || ( wayPointArray[n].type != eWayPointType::OutsideErasureArea ) )
                           {
                               if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                               {
                                   FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                                   ::ULIS::FVec2D *bezier = cubicSegment->GetBezier();
                                   FOdysseyVectorSegment* newSegment = nullptr;
                                   ::ULIS::FVec2D  sample[4];

                                   FOdysseyVector::BezierExtract( bezier[0]
                                                                , bezier[1]
                                                                , bezier[2]
                                                                , bezier[3]
                                                                , wayPointArray[i].t
                                                                , wayPointArray[n].t
                                                                , sample[0]
                                                                , sample[1]
                                                                , sample[2]
                                                                , sample[3] );

                                   newSegment = new FOdysseyVectorSegmentCubic( mPath
                                                                              , wayPointArray[i].vertex
                                                                              , sample[1].x
                                                                              , sample[1].y
                                                                              , sample[2].x
                                                                              , sample[2].y
                                                                              , wayPointArray[n].vertex
                                                                              , true );

                                   oAddedSegmentArray.push_back( newSegment );

                                  // mPath->AddSegment( newSegment );

                                   //newSegmentArray.push_back( newSegment );
                               }
                           }
                       }
                   }

                   return false;
               } );

    return hasHit;
}

bool
FOdysseyVectorChain::HitMask( BLContext* iBLContex
                            , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                            , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                            , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                            , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray )
{
    // the mask image must be selected by the vector engine at this point
    BLImage* blimg = mPath->GetEngine()->GetBLMask();

    if( blimg )
    {
        BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
        BLImageData imageData;

        blimg->getData( &imageData );

        return Trace( &imageData
                    , oAddedVertexArray
                    , oAddedSegmentArray
                    , oRemovedVertexArray
                    , oRemovedSegmentArray );
    }

    return false;
}
