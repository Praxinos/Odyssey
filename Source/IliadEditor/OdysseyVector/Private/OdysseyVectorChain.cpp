// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
                          , uint32 iIndexWayPoint1
                          , bool iErased )
{
    double t0, t1;

    segment = iSegment;
    // work with indexes because the wayPoint array is gonna be resized, so
    // we can't work with pointers
    indexWayPoint0 = iIndexWayPoint0;
    indexWayPoint1 = iIndexWayPoint1;

    erased = iErased;

    if( iWayPointArray[indexWayPoint0].flags & FWayPoint::Original )
    {
        t0 = iWayPointArray[indexWayPoint0].vertex->GetIndex( iSegment );
    }
    else
    {
        t0 = iWayPointArray[indexWayPoint0].t;
    }

    if( iWayPointArray[indexWayPoint1].flags & FWayPoint::Original )
    {
        t1 = iWayPointArray[indexWayPoint1].vertex->GetIndex( iSegment );
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

std::vector<FOdysseyVectorVertex*>&
FOdysseyVectorChain::GetVertexArray()
{
    return mVertexArray;
}

std::vector<FOdysseyVectorSegment*>&
FOdysseyVectorChain::GetSegmentArray()
{
    return mSegmentArray;
}

FOdysseyVectorChain::FOdysseyVectorChain( FOdysseyVectorPath* iPath
                                        , FOdysseyVectorVertex* iUnchainedVertex )
    : mPath( iPath )
    , mLength( 0.0f )
    , mBBox( 0.0f, 0.0f, 0.0f, 0.0f )
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

static void
GetNextErasableSectionLinkInfo( FOdysseyVectorVertex* iVertex
                              , FOdysseyVectorPath* iPath
                              , FSectionLinkInfo* iLastSectionInfo
                              , std::vector<FSectionLinkInfo*>& oSectionLinkInfoArray )
{
    for( FSectionLinkInfo& sectionLinkInfo : iVertex->GetSectionLinkInfoList() )
    {
        if( sectionLinkInfo.section->GetSegment()->GetOwner() == iPath )
        {
            if( sectionLinkInfo.section != iLastSectionInfo->section )
            {
                oSectionLinkInfoArray.push_back( &sectionLinkInfo );
            }
        }
    }
}

// static
void
FOdysseyVectorChain::ExtendErasedSection( FOdysseyVectorPath* iPath
                                        , FSectionLinkInfo* iLastSectionInfo )
{
    std::vector<FSectionLinkInfo*> sectionLinkInfoArray;

    sectionLinkInfoArray.push_back( iLastSectionInfo );

    while( sectionLinkInfoArray.size() )
    {
        std::vector<FSectionLinkInfo*> nextSectionLinkInfoArray;

        for( FSectionLinkInfo* sectionLinkInfo : sectionLinkInfoArray )
        {
            FOdysseyVectorSection* currentSection =  sectionLinkInfo->section;
            FOdysseyVectorVertex* currentVertex = currentSection->GetVertex( sectionLinkInfo->sectionVertexIndex );
            FOdysseyVectorVertex* nextVertex = currentSection->GetOtherVertex( currentVertex );

            sectionLinkInfo->section->SetErased( true );

            if( ( nextVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
             && ( sectionLinkInfo->section->IsGap() == false ) )
            {
                GetNextErasableSectionLinkInfo( nextVertex
                                              , iPath
                                              , sectionLinkInfo
                                              , nextSectionLinkInfoArray );
            }
        }

        sectionLinkInfoArray = nextSectionLinkInfoArray;
    }
}

bool
FOdysseyVectorChain::PickSection( FOdysseyVectorSection* iSection
                                , const ::ULIS::FRectD& iMaskRect
                                , const uint8* iMaskPixelData )
{
    ::ULIS::FVec2D* bezier = iSection->GetBezier();
    // Note, section are in paingroup coordinates (path's parent), not in path coordinates.
    BLMatrix2D& worldMatrix = iSection->GetOwner()->GetWorldMatrix();
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

void
FOdysseyVectorChain::GetSections( FOdysseyVectorVertexIntersection* iIntersectionVertex
                                , FOdysseyVectorSegment* iSegment
                                , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    for( FSectionLinkInfo& sectionLinkInfo : iIntersectionVertex->GetSectionLinkInfoList() )
    {
        if( sectionLinkInfo.section->GetSegment() == iSegment )
        {
            oSectionArray.push_back( sectionLinkInfo.section );
        }
    }
}

void
FOdysseyVectorChain::GetSections( FOdysseyVectorVertex* iVertex
                                , FOdysseyVectorPath* iPath
                                , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    for( FSectionLinkInfo& sectionLinkInfo : iVertex->GetSectionLinkInfoList() )
    {
        if( sectionLinkInfo.section->GetSegment()->GetOwnerAsPath() == iPath )
        {
            oSectionArray.push_back( sectionLinkInfo.section );
        }
    }
}

uint32
FOdysseyVectorChain::GetErasureFlags( FOdysseyVectorVertex* iVertex )
{
    uint32 erasedSectionCount = iVertex->GetErasedSectionCount( mPath );
    uint32 sectionCount = iVertex->GetSectionCount( mPath );

    if( erasedSectionCount == 0 )
    {
        return FWayPoint::OutsideErasureArea;
    }

    if( erasedSectionCount && ( erasedSectionCount != sectionCount ) )
    {
        return FWayPoint::BordersErasureArea;
    }

    if( erasedSectionCount && ( erasedSectionCount == sectionCount ) )
    {
        return FWayPoint::InsideErasureArea;
    }

    return 0;
}

bool
FOdysseyVectorChain::EraseSections( BLImageData* iImageData
                                  , std::vector<FWayPoint>& oWayPointArray
                                  , std::vector<FWayFragment>& oWayFragmentArray )
{
    BLMatrix2D& pathInverseWorldMatrix = mPath->GetInverseWorldMatrix();
    std::vector<FOdysseyVectorSection*> pickedSectionArray;
    uint32 erasureFlag = FWayPoint::OutsideErasureArea;
    bool hasHit = false;
    FOdysseyVectorVertex* firstVertex = mVertexArray[0];
    FOdysseyVectorSegment* firstSegment = mSegmentArray[0];
    FOdysseyVectorVertex* currentVertex = firstVertex;

    oWayPointArray.clear();
    // reserve 1 point per vertex + 2 point per segment
    oWayPointArray.reserve( mVertexArray.size() + ( mSegmentArray.size() * 2 ) );

    oWayFragmentArray.clear();
    // reserve 3 segment per segment
    oWayFragmentArray.reserve( mSegmentArray.size() * 3 );

    // record the first vertex
    currentVertex->SetID( oWayPointArray.size() );

    erasureFlag = GetErasureFlags( currentVertex );

    if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
    {
        hasHit = true;
    }

    oWayPointArray.emplace_back( currentVertex, erasureFlag | FWayPoint::Original );

    // record subsequent vertices
    for( FOdysseyVectorSegment* segment : mSegmentArray )
    {
        FOdysseyVectorVertex* segmentVertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* segmentVertex1 = segment->GetVertex(1);
        FOdysseyVectorSection* sectionBuffer = segment->GetSectionBuffer();
        uint32 sectionCount = segment->GetSectionCount();
        uint32 startIndex = ( segmentVertex0 == currentVertex ) ? 0 : ( sectionCount - 1 );
        int32 stepIndex = ( segmentVertex0 == currentVertex ) ?  1 : - 1;
        uint32 nextVertexIndex = ( segmentVertex0 == currentVertex ) ? 1 : 0;


        for( uint32 i = 0, n = startIndex; i < sectionCount; i++, n += stepIndex )
        {
            FOdysseyVectorSection* section = &sectionBuffer[n];
            FOdysseyVectorVertex* nextVertex = section->GetOtherVertex( currentVertex );
            FOdysseyVectorVertex* chainedVertex = oWayPointArray.back().vertex;

            erasureFlag = GetErasureFlags( nextVertex );

            if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
            {
                hasHit = true;
            }

            if( nextVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                // for indexing
                nextVertex->SetID( oWayPointArray.size() );

                oWayPointArray.emplace_back( nextVertex, erasureFlag | FWayPoint::Original );

                // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                oWayFragmentArray.emplace_back( segment
                                              , oWayPointArray
                                              , chainedVertex->GetID()
                                              , nextVertex->GetID()
                                              , section->IsErased() );
            }

            if( nextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(nextVertex);

                if( erasureFlag & FWayPoint::BordersErasureArea )
                {
                    double t = section->GetSegmentT( nextVertexIndex );

                    double radius = ( t * segmentVertex1->GetRadius() ) + ( ( 1.0f - t ) * segmentVertex0->GetRadius() );
                    ::ULIS::FVec2D& vertexCoords = nextVertex->GetCoords();
                    // owner is the paintgroup
                    BLMatrix2D& ownerWorldMatrix = nextVertex->GetOwner()->GetWorldMatrix();
                    BLPoint vertexWorldCoords = ownerWorldMatrix.mapPoint( vertexCoords.x
                                                                         , vertexCoords.y );
                    BLPoint vertexPathCoords  = pathInverseWorldMatrix.mapPoint( vertexWorldCoords.x
                                                                               , vertexWorldCoords.y );
                    FOdysseyVectorVertex* derivedVertex = new FOdysseyVectorVertex( vertexPathCoords.x
                                                                                  , vertexPathCoords.y
                                                                                  , radius );

                    // for indexing
                    derivedVertex->SetID( oWayPointArray.size() );

                    oWayPointArray.emplace_back( derivedVertex
                                               , intersectionVertex
                                               , erasureFlag
                                               , std::clamp<double>( t, 0.001f, 0.999f ) );

                    // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                    oWayFragmentArray.emplace_back( segment
                                                  , oWayPointArray
                                                  , chainedVertex->GetID()
                                                  , derivedVertex->GetID()
                                                  , section->IsErased() );
                }
            }

            currentVertex = nextVertex;
        }
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
    FWayPoint* chainedWayPoint = iChainedWayPoint;

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

                FWayPoint wayPoint = FWayPoint( newVertex
                                              , nullptr
                                              , FWayPoint::BordersErasureArea
                                              , t );

                oWayPointArray.push_back( wayPoint );

                newVertex->SetID( wayPointCount );
                // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                oWayFragmentArray.emplace_back( iSegment
                                              , oWayPointArray
                                              , chainedWayPoint->vertex->GetID()
                                              , newVertex->GetID()
                                              , lastAlphaValue ? true : false );
                // prepare the next iteration
                chainedWayPoint = &oWayPointArray.back();

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

                FWayPoint wayPoint = FWayPoint( newVertex
                                              , nullptr
                                              , FWayPoint::BordersErasureArea
                                              , t );

                oWayPointArray.push_back( wayPoint );

                newVertex->SetID( wayPointCount );
                // record waypoints ID because the array might grow (thus the pointer would change)
                oWayFragmentArray.emplace_back( iSegment
                                              , oWayPointArray
                                              , chainedWayPoint->vertex->GetID()
                                              , newVertex->GetID()
                                              , lastAlphaValue ? true : false );
                // prepare the next iteration
                chainedWayPoint = &oWayPointArray.back();

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

    return chainedWayPoint;
}

bool
FOdysseyVectorChain::EraseSegments( BLImageData* iImageData
                                  , std::vector<FWayPoint>& oWayPointArray
                                  , std::vector<FWayFragment>& oWayFragmentArray )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
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

    IterateSegments( [ this
                     , iImageData
                     , &oWayPointArray
                     , &oWayFragmentArray
                     , &hasHit
                     , &worldMatrix ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
    {
        std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
        FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex( vertex );
        bool revert = ( vertex == segment->GetVertex(0) ) ? false : true;
        FWayPoint* firstChainedWayPoint = &oWayPointArray[vertex->GetID()];
        FWayPoint* lastChainedWayPoint = &oWayPointArray[otherVertex->GetID()];
        FWayPoint* chainedWayPoint = firstChainedWayPoint;
        uint32 hitCount = 0;

        // iteration
        if( revert == false )
        {
            for( auto it = fractionCache.begin(); it != fractionCache.end(); ++it )
            {
                FOdysseyVectorFraction& fraction = *it;
                ::ULIS::FVec2D& p0Coords = fraction.point[0]->GetCoords();
                ::ULIS::FVec2D& p1Coords = fraction.point[1]->GetCoords();

                BLPoint p0 = worldMatrix.mapPoint( p0Coords.x, p0Coords.y );
                BLPoint p1 = worldMatrix.mapPoint( p1Coords.x, p1Coords.y );

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
                                         , lastChainedWayPoint->vertex->GetID()
                                         , ( lastChainedWayPoint->flags & FWayPoint::InsideErasureArea ) ? true : false );
        }
        else
        {
            for( auto it = fractionCache.rbegin(); it != fractionCache.rend(); ++it )
            {
                FOdysseyVectorFraction& fraction = *it;
                ::ULIS::FVec2D& p0Coords = fraction.point[0]->GetCoords();
                ::ULIS::FVec2D& p1Coords = fraction.point[1]->GetCoords();

                BLPoint p0 = worldMatrix.mapPoint( p0Coords.x, p0Coords.y );
                BLPoint p1 = worldMatrix.mapPoint( p1Coords.x, p1Coords.y );

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
                                         , lastChainedWayPoint->vertex->GetID()
                                         , ( lastChainedWayPoint->flags & FWayPoint::InsideErasureArea ) ? true : false );
        }

        return false; // keep iterating;
    } );

    //---------------- parse hits if any ---------------------//
    if( oWayPointArray.size() > mVertexArray.size() )
    {
        hasHit = true;
    }

    return hasHit;
}
