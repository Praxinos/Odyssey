#include "OdysseyVectorChain.h"
#include "OdysseyVector.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegmentCubic.h"

FWayFragment::FWayFragment( FOdysseyVectorSegment* iSegment
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
FOdysseyVectorChain::IterateSections( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSection*)> iCallback )
{
    FOdysseyVectorVertex* firstVertex = mVertexArray.front();
    FOdysseyVectorVertex* lastVertex = mVertexArray.back();
    // this ensures we won't get section from a gap segment. Gap segments are not linked to vertices
    FOdysseyVectorSegment* lastSegment = mSegmentArray.back();
    FOdysseyVectorSection* lastSection = lastVertex->GetSection( lastSegment );
    FOdysseyVectorVertex* currentVertex = firstVertex;
    FOdysseyVectorSection* currentSection = currentVertex->GetSection( mSegmentArray.front() );

    while( currentSection )
    {
        FOdysseyVectorVertex* nextVertex = currentSection->GetOtherVertex( currentVertex );

        if( iCallback( currentVertex, currentSection ) == true )
        {
            return;
        }

        if( currentSection != lastSection )
        {
            FOdysseyVectorSection* nextSection = nextVertex->GetOtherSection( currentSection, false );

            currentVertex = nextVertex;
            currentSection = nextSection;
        }
        else
        {
            currentVertex = nullptr;
            currentSection = nullptr;
        }
    }
}

// static
void
FOdysseyVectorChain::ExtendErasedSection( FOdysseyVectorVertex* iVertex
                                        , FOdysseyVectorSection* iFromSection )
{
    FOdysseyVectorVertex* currentVertex = iVertex;
    FOdysseyVectorSection* currentSection = iFromSection;

    while(  currentSection
       && ( currentSection->GetSegment()->GetClass() != FOdysseyVectorSegmentCubicGap::StaticClass()    )
       && ( currentVertex->GetClass()                != FOdysseyVectorVertexIntersection::StaticClass() ) )
    {
        FOdysseyVectorVertex* nextVertex = currentSection->GetOtherVertex( currentVertex );

        currentSection->SetErased( true );

        if( currentSection != iFromSection )
        {
            FOdysseyVectorSection* nextSection = nextVertex->GetOtherSection( currentSection, false );

            currentVertex = nextVertex;
            currentSection = nextSection;
        }
        else
        {
            currentVertex = nullptr;
            currentSection = nullptr;
        }
    }
}

bool
FOdysseyVectorChain::PickSection( FOdysseyVectorSection* iSection
                                , const ::ULIS::FRectD& iMaskRect
                                , const uint8* iMaskPixelData )
{
    ::ULIS::FVec2D* bezier = iSection->GetBezier();
    // Note, section are in paingroup coordinates (path's parent), not in path coordinates.
    BLMatrix2D& worldMatrix = mPath->GetParent()->GetWorldMatrix();
    BLPoint pt[4] = { worldMatrix.mapPoint( bezier[0].x, bezier[0].y )
                    , worldMatrix.mapPoint( bezier[1].x, bezier[1].y )
                    , worldMatrix.mapPoint( bezier[2].x, bezier[2].y )
                    , worldMatrix.mapPoint( bezier[3].x, bezier[3].y ) };
    ::ULIS::FVec2D worldBezier[4] = { ::ULIS::FVec2D( pt[0].x, pt[0].y )
                                    , ::ULIS::FVec2D( pt[1].x, pt[1].y )
                                    , ::ULIS::FVec2D( pt[2].x, pt[2].y )
                                    , ::ULIS::FVec2D( pt[3].x, pt[3].y ) };

    return FOdysseyVector::PickBezier( worldBezier, iMaskRect, iMaskPixelData );
}

bool
FOdysseyVectorChain::PickSections( std::vector<FOdysseyVectorSection*>& oPickedSectionArray )
{
    BLImage* maskImage = mPath->GetEngine()->GetBLMask();
    BLImageData maskData;
    ::ULIS::FRectD maskRect;

    oPickedSectionArray.clear();

    maskImage->getData( &maskData );

    maskRect = ::ULIS::FRectD( 0, 0, maskData.size.w, maskData.size.h );

    IterateSections( [ this
                     , &maskRect
                     , &maskData
                     , &oPickedSectionArray ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSection* section ) -> bool
    {
        if( PickSection( section, maskRect, (uint8*) maskData.pixelData ) )
        {
            oPickedSectionArray.push_back( section );
        }

        return false; // keep iterating
    } );

    return oPickedSectionArray.size() ? true : false;
}

bool
FOdysseyVectorChain::EraseSections( BLImageData* iImageData
                                  , std::vector<FWayPoint>& oWayPointArray
                                  , std::vector<FWayFragment>& oWayFragmentArray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    std::vector<FOdysseyVectorSection*> pickedSectionArray;
    std::vector<FWaySection> waySectionArray;
    bool hasHit = false;

    PickSections( pickedSectionArray );

    if( pickedSectionArray.size() )
    {
        // second step. Extend erased section array with the neighbour sections until we reach
        // the end of the chain or an intersection
        for( FOdysseyVectorSection* pickedSection : pickedSectionArray )
        {
            pickedSection->SetErased( true );

            // static call
            FOdysseyVectorChain::ExtendErasedSection( pickedSection->GetVertex(0), pickedSection );
            // static call
            FOdysseyVectorChain::ExtendErasedSection( pickedSection->GetVertex(1), pickedSection );
        }

        oWayPointArray.clear();
        // reserve 1 point per vertex + 2 point per segment
        oWayPointArray.reserve( mVertexArray.size() + ( mSegmentArray.size() * 2 ) );

        // reserve 6 section per segment
        waySectionArray.reserve( mSegmentArray.size() * 6 );

        oWayFragmentArray.clear();
        // reserve 3 segment per segment
        oWayFragmentArray.reserve( mSegmentArray.size() * 3 );

        // prepare indexes and determine if original vertices should be kept
        for( int i = 0; i < mVertexArray.size(); i++ )
        {
            FOdysseyVectorVertex* vertex = mVertexArray[i];

            if( vertex->HasErasedSectionsOnly() == false ) // vertex in "dark" zone, keep it
            {
                oWayPointArray.emplace_back( vertex
                                           , FWayPoint::OutsideErasureArea
                                           | FWayPoint::Original );
            }
            else
            {
                oWayPointArray.emplace_back( vertex
                                           , FWayPoint::InsideErasureArea
                                           | FWayPoint::Original );

                hasHit = true;
            }
            // for indexing
            vertex->SetID( i );
        }

        IterateSections( [ this
                         , &oWayPointArray
                         , &waySectionArray
                         , &hasHit ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSection* section ) -> bool
        {
            FOdysseyVectorVertex* nextVertex = section->GetOtherVertex( vertex );
            bool revert = ( vertex == section->GetVertex(0) ) ? false : true;
            uint32 hitCount = 0;

            if( nextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                // Note: nextSection cannot be NULL, as an intersection vertex cannot be the last
                FOdysseyVectorSection* nextSection = nextVertex->GetOtherSection( section, false );
                double t = nextVertex->GetT( section->GetSegment() );
                uint32 wayPointCount = oWayPointArray.size();

                if( ( section->IsErased() == false ) && ( nextSection->IsErased() == false ) )
                {
                    oWayPointArray.emplace_back( nextVertex, FWayPoint::OutsideErasureArea, t );
                }

                if( ( section->IsErased() == true ) && ( nextSection->IsErased() == false ) )
                {
                    oWayPointArray.emplace_back( nextVertex, FWayPoint::LeavesErasureArea, t );
                }

                if( ( section->IsErased() == true ) && ( nextSection->IsErased() == true ) )
                {
                    oWayPointArray.emplace_back( nextVertex, FWayPoint::InsideErasureArea, t );
                }

                if( ( section->IsErased() == false ) && ( nextSection->IsErased() == true ) )
                {
                    oWayPointArray.emplace_back( nextVertex, FWayPoint::EntersErasureArea, t );
                }

                nextVertex->SetID( wayPointCount );
            }

            // Note: waySections use waypoints ID because the array might grow (thus the pointer would change)
            waySectionArray.emplace_back( section, vertex->GetID(), nextVertex->GetID() );


            //---------------- parse hits if any ---------------------//
            if( section->IsErased() == true )
            {
                hasHit = true;
            }

            return false; // keep iterating;
        } );

        // second pass will build segment fragments from sections.
        IterateSegments( [ this
                         , &oWayPointArray
                         , &waySectionArray
                         , &oWayFragmentArray
                         , &hasHit ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
        {
            FWayPoint* firstWayPoint = &oWayPointArray[vertex->GetID()];

            // check among all sections ...
            for( FWaySection& waySection : waySectionArray )
            {
                // ... the sections belonging to this segment. They are consecutive.
                if( waySection.section->GetSegment() == segment )
                {
                    FWayPoint* secondWayPoint = &oWayPointArray[waySection.indexWayPoint1];

                    if( secondWayPoint->flags & FWayPoint::EntersErasureArea )
                    {
                        // only vertex at intersection can have the EntersErasureArea flag
                        // the cast is only for clarity
                        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(secondWayPoint->vertex);
                        ::ULIS::FVec2D& coords = intersectionVertex->GetCoords();
                        FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( coords.x
                                                                                  , coords.y
                                                                                  , 1.0f );

                        newVertex->SetID( intersectionVertex->GetID() );
                        // replace intersection vertex with this new vertex
                        secondWayPoint->vertex = newVertex;

                        oWayFragmentArray.emplace_back( segment
                                                      , oWayPointArray
                                                      , firstWayPoint ->vertex->GetID()
                                                      , secondWayPoint->vertex->GetID() );

                        firstWayPoint = secondWayPoint;
                    }

                    if( secondWayPoint->flags & FWayPoint::LeavesErasureArea )
                    {
                        // only vertex at intersection can have the LeavesErasureArea flag
                        // the cast is only for clarity
                        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(secondWayPoint->vertex);
                        ::ULIS::FVec2D& coords = intersectionVertex->GetCoords();
                        FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( coords.x
                                                                                  , coords.y
                                                                                  , 1.0f );

                        newVertex->SetID( intersectionVertex->GetID() );
                        // replace intersection vertex with this new vertex
                        secondWayPoint->vertex = newVertex;

                        oWayFragmentArray.emplace_back( segment
                                                      , oWayPointArray
                                                      , firstWayPoint ->vertex->GetID()
                                                      , secondWayPoint->vertex->GetID() );

                        firstWayPoint = secondWayPoint;
                    }

                    if( secondWayPoint->flags & FWayPoint::Original )
                    {
                        oWayFragmentArray.emplace_back( segment
                                                      , oWayPointArray
                                                      , firstWayPoint ->vertex->GetID()
                                                      , secondWayPoint->vertex->GetID() );

                        firstWayPoint = secondWayPoint;
                    }
                }
            }

            return false; // keep iterating;
        } );
    }

    return hasHit;
}

// callback must return false to keep iterating
void
FOdysseyVectorChain::IterateSegments( std::function<bool( FOdysseyVectorVertex*, FOdysseyVectorSegment*)> iCallback )
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
                              , std::vector<FWayFragment>& oWayFragmentArray
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
                // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                oWayFragmentArray.emplace_back( iSegment, oWayPointArray, iChainedWayPoint->vertex->GetID(), newVertex->GetID() );
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
                oWayFragmentArray.emplace_back( iSegment, oWayPointArray, iChainedWayPoint->vertex->GetID(), newVertex->GetID() );
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
FOdysseyVectorChain::EraseSegments( BLImageData* iImageData
                                  , std::vector<FWayPoint>& oWayPointArray
                                  , std::vector<FWayFragment>& oWayFragmentArray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    FWayPoint* chainedWayPoint;
    bool hasHit = false;

    oWayPointArray.clear();
    // reserve 1 point per vertex + 2 point per segment
    oWayPointArray.reserve( mVertexArray.size() + ( mSegmentArray.size() * 2 ) );

    oWayFragmentArray.clear();
    // reserve 3 segment per segment
    oWayFragmentArray.reserve( mSegmentArray.size() * 3 );

    // prepare indexes and determine if original vertices should be kept
    for( int i = 0; i < mVertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = mVertexArray[i];
        ::ULIS::FVec2D& coords = vertex->GetCoords();
        BLPoint point = worldMatrix.mapPoint( coords.x, coords.y );
        uint8 alpha = GetAlpha( point.x, point.y, iImageData );

        if( alpha == 0 ) // vertex in dark zone, keep it
        {
            oWayPointArray.emplace_back( vertex
                                       , FWayPoint::OutsideErasureArea
                                       | FWayPoint::Original );
        }
        else
        {
            oWayPointArray.emplace_back( vertex
                                       , FWayPoint::InsideErasureArea
                                       | FWayPoint::Original );

            hasHit = true;
        }

        vertex->SetID( i );
    }

    chainedWayPoint = &oWayPointArray[0];

    IterateSegments( [ this
                     , iImageData
                     , &chainedWayPoint
                     , &oWayPointArray
                     , &oWayFragmentArray
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
                                           , oWayFragmentArray
                                           , segment
                                           , revert );
            }
            // don't forget the last sub-segment
            oWayFragmentArray.emplace_back( segment
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
                                           , oWayFragmentArray
                                           , segment
                                           , revert );
            }
            // don't forget the last sub-segment
            oWayFragmentArray.emplace_back( segment
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
