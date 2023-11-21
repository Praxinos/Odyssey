#include "OdysseyVectorChain.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"

FOdysseyVectorChain::~FOdysseyVectorChain()
{
}

FOdysseyVectorChain::FOdysseyVectorChain( FOdysseyVectorPath* iPath
                                        , FOdysseyVectorVertex* iInitiatorVertex )
    : mInitiatorVertex( iInitiatorVertex )
    , mLength( 0.0f )
    , mBBox( 0.0f, 0.0f, 0.0f, 0.0f )
{
    mSegmentArray.reserve( iPath->GetSegmentList().size() );
}

// callback must return false to keep iterating
uint32
FOdysseyVectorChain::Iterate( std::function<bool( FOdysseyVectorVertex*
                                                , FOdysseyVectorSegment*)> iCallback )
{
    FOdysseyVectorVertex* currentVertex = mVertex;

    for( FOdysseyVectorSegment* segment : mSegmentArray )
    {
        FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( currentVertex );

        iCallback( currentVertex, segment );

        currentVertex = nextVertex;
    }
}

// returns the number of time we hit an alpha boundary
uint32
FOdysseyVectorChain::TraceLine( uint8 iLastAlphaValue
                              , int32
                              , int32
                              , double
                              , int32
                              , int32
                              , double
                              , BLImageData*
                              , std::vector<FWayPoint>& oWayPointArray )
{

}

uint32
FOdysseyVectorChain::HitMask( BLContext* iBLContext )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();

    Iterate( [ &worldMatrix ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
             {
                 std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();

                if( vertex == segment->GetVertex(0) )
                { 
                    // iteration
                    for( auto it = fractionCache.begin(); it != fractionCache.end(); ++it )
                    {
                        FOdysseyVectorFraction& fraction = *it;

                        BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                        BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                        TraceLine( lastAlphaValue, p0.x, p0.y, p1.x, p1.y, fromT, toT, imageData )
                    }
                 }
                 else
                 {
                    // iteration for inverted segments
                    for( auto it = fractionCache.rbegin(); it != fractionCache.rend(); ++it )
                    {
                        FOdysseyVectorFraction& fraction = *it;

                        BLPoint p0 = worldMatrix.mapPoint( fraction.lineVertex[0].x, fraction.lineVertex[0].y );
                        BLPoint p1 = worldMatrix.mapPoint( fraction.lineVertex[1].x, fraction.lineVertex[1].y );

                        TraceLine( lastAlphaValue, p1.x, p1.y, p0.x, p0.y, toT, fromT, imageData )
                    }
                 }
             } );
}

uint32
FOdysseyVectorChain::HitMask( BLContext* iBLContext )
{
    // the mask image must be selected by the vector engine at this point
    BLImage* blimg = mPath->GetEngine()->GetBLMask();

    if( blimg )
    {
        BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
        BLImageData imageData;

        for( FOdysseyVectorSegment* segment : mSegmentArray )
        {
            std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
            ::ULIS::FVec2D& firstCoords = mVertex->GetCoords();
            BLPoint firstAt = worldMatrix.mapPoint( firstCoords.x, firstCoords.y );
            std::vector<FOdysseyVectorSegment*> subSegmentArray;
            std::vector<double> subVertexT;
            int32 previousPixelValue;
            bool hasHit = false;

            for( uint32 i = 0; i < fractionCache.size(); i++ )
            {
                BLPoint p0 = worldMatrix.mapPoint( fractionCache[i].lineVertex[0].x, fractionCache[i].lineVertex[0].y );
                BLPoint p1 = worldMatrix.mapPoint( fractionCache[i].lineVertex[1].x, fractionCache[i].lineVertex[1].y );

                TraceLine( p0.x, p0.y, fractionCache[i].fromT
                         , p1.x, p1.y, fractionCache[i].toT
                         , [ segment
                           , &imageData
                           , &previousPixelValue
                           , &subVertexT
                           , &subSegmentArray
                           , &newVertexArray
                           , &hasHit]( int32 iX, int32 iY, double iT) -> bool
                           {
                                /*if( ( iX >= 0 && iX < imageData.size.w )
                                && ( iY >= 0 && iY < imageData.size.h ) )
                                {*/
                                  uint8 *pixel = static_cast<uint8*>(imageData.pixelData);
                                  uint32 offset = ( iY * imageData.size.w ) + iX;
                                  int32 pixelValue = ( ( iX >= 0 && iX < imageData.size.w )
                                                    && ( iY >= 0 && iY < imageData.size.h ) ) ? pixel[offset] : 0;

                                  if( pixelValue == 255 ) hasHit = true;

                                  if( iT == 0.0f )
                                  {
                                      if( pixelValue == 0 )
                                      {
                                          subVertexT.push_back( iT );
                                      }

                                      previousPixelValue = pixelValue;
                                  }

                                  if( ( iT > 0.0f ) && ( iT < 1.0f ) )
                                  {
                                      if( (int32) abs( pixelValue - previousPixelValue ) == 255 )
                                      {
                                          subVertexT.push_back( iT );

                                          previousPixelValue = pixelValue;
                                      }
                                  }

                                  if( iT == 1.0f )
                                  {
                                      if( pixelValue == 0 )
                                      {
                                          subVertexT.push_back( iT );
                                      }
                                  }

                                  if( subVertexT.size() == 2 )
                                  {
                                      uint32 tCount = subVertexT.size();
                                      double t0 = subVertexT[0];
                                      double t1 = subVertexT[1];

                                      if( fabs( subVertexT[0] - subVertexT[1]) < 1.0f )
                                      {
                                          subSegmentArray.push_back( segment->Sample( t0, t1, newVertexArray ) );
                                      }

                                      subVertexT.clear();
                                  }
                                /*}*/

                                // keep tracing the line
                                return false;
                            });
            }

            if( hasHit )
            {
                // won't insert anything if no subsegment were created
                if( subSegmentArray.size() )
                {
                    newSegmentArray.insert( newSegmentArray.end(), subSegmentArray.begin(), subSegmentArray.end() );
                }

                // in case of a hit, old segment is deleted no matter what.
                oldSegmentArray.push_back( segment );
            }
        }
    }
}
