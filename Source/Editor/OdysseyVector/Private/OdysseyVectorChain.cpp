#include "OdysseyVectorChain.h"
#include "OdysseyVector.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
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

static
FOdysseyVectorSection* GetNextSection( FOdysseyVectorVertex* iVertex
                                     , FOdysseyVectorSection* iLastSection )
{
    for( FSectionLinkInfo& sectionLinkInfo : iVertex->GetSectionLinkInfoList() )
    {
        if( sectionLinkInfo.section != iLastSection )
        {
            FOdysseyVectorObject* sectionSegmentOwner = sectionLinkInfo.section->GetSegment()->GetOwner();

            if( sectionSegmentOwner->GetClass() == FOdysseyVectorPath::StaticClass() )
            {
                if( ( iLastSection == nullptr )
                 || ( sectionSegmentOwner == iLastSection->GetSegment()->GetOwner() ) )
                {
                    return sectionLinkInfo.section;
                }
            }
        }
    }

    return nullptr;
}

// static
void
FOdysseyVectorChain::ExtendErasedSection( FOdysseyVectorVertex* iVertex
                                        , FOdysseyVectorSection* iSection )
{
    FOdysseyVectorVertex* currentVertex = iVertex;
    FOdysseyVectorSection* currentSection = iSection;

    while( currentSection )
    {
        FOdysseyVectorVertex* nextVertex = currentSection->GetOtherVertex( currentVertex );
        // Note: GetNextSection will return nullptr when meeting a gap section
        FOdysseyVectorSection* nextSection = nullptr;

        currentSection->SetErased( true );

 //UE_LOG(LogTemp, Warning, TEXT("marking section: %d:%d"), currentSection, currentVertex->GetSectionCount() );

        if( nextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
        {
            break;
        }
        else
        {
            nextSection = GetNextSection( nextVertex, currentSection );
        }

        if( nextSection != iSection ) // check loop
        {
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
FOdysseyVectorChain::GetErasureFlags( FOdysseyVectorSection* iPrevSection
                                    , FOdysseyVectorVertex* iVertex
                                    , FOdysseyVectorSection* iNextSection )
{
    if( iPrevSection == nullptr )
    {
        return iNextSection->IsErased() ? FWayPoint::EntersErasureArea
                                        : FWayPoint::OutsideErasureArea;
    }

    if( iNextSection == nullptr )
    {
        return iPrevSection->IsErased() ? FWayPoint::LeavesErasureArea
                                        : FWayPoint::OutsideErasureArea;
    }

    if( ( iPrevSection->IsErased() == false ) && ( iNextSection->IsErased() == false ) )
    {
        return FWayPoint::OutsideErasureArea;
    }

    if( ( iPrevSection->IsErased() == true  ) && ( iNextSection->IsErased() == false ) )
    {
        return FWayPoint::LeavesErasureArea;
    }

    if( ( iPrevSection->IsErased() == true  ) && ( iNextSection->IsErased() == true  ) )
    {
        return FWayPoint::InsideErasureArea;
    }

    if( ( iPrevSection->IsErased() == false ) && ( iNextSection->IsErased() == true  ) )
    {
        return FWayPoint::EntersErasureArea;
    }

    return 0;
}

// FSectionLinkInfo* sectionLinkInfo = sectionNextVertex->GetSectionLinkInfo( iSection, sectionNextVertexIndex );
FSectionLinkInfo*
FOdysseyVectorChain::GetNextSectionLinkInfo( FOdysseyVectorSection* iLastSection
                                           , FOdysseyVectorVertex* iLastSectionVertex
                                           , uint32 iLastSectionVertexIndex )
{
    ::ULIS::FVec2D vector = iLastSection->GetVectorFromVertex( iLastSectionVertexIndex, false, false );

    if( iLastSectionVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        for( FSectionLinkInfo& nextSectionLinkInfo : iLastSectionVertex->GetSectionLinkInfoList() )
        {
            FOdysseyVectorSegment* nextSectionSegment = nextSectionLinkInfo.section->GetSegment();

            if( nextSectionSegment != iLastSection->GetSegment() )
            {
                return &nextSectionLinkInfo;
            }
        }
    }

    if( iLastSectionVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
    {
        for( FSectionLinkInfo& nextSectionLinkInfo : iLastSectionVertex->GetSectionLinkInfoList() )
        {
            FOdysseyVectorSegment* nextSectionSegment = nextSectionLinkInfo.section->GetSegment();
            FOdysseyVectorSection* nextSection = nextSectionLinkInfo.section;
            uint32 nextSectionVertexIndex = nextSectionLinkInfo.sectionVertexIndex;

            if( nextSectionSegment == iLastSection->GetSegment() )
            {
                if( iLastSection->GetSegmentT( iLastSectionVertexIndex ) == nextSection->GetSegmentT( nextSectionVertexIndex ) )
                {
                    if( nextSection != iLastSection )
                    {
                        return &nextSectionLinkInfo;
                    }
                }
            }
        }
    }

    return nullptr;
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
    FSectionLinkInfo* firstSectionLinkInfo = firstVertex->GetSectionLinkInfo( firstSegment );
    FSectionLinkInfo* sectionLinkInfo = firstSectionLinkInfo;
    FOdysseyVectorSegment* prevSegment = firstVertex->GetOtherSegment( firstSegment );
    FSectionLinkInfo* prevSectionLinkInfo = prevSegment ? firstVertex->GetSectionLinkInfo( prevSegment )
                                                        : nullptr ;


    oWayPointArray.clear();
    // reserve 1 point per vertex + 2 point per segment
    oWayPointArray.reserve( mVertexArray.size() + ( mSegmentArray.size() * 2 ) );

    oWayFragmentArray.clear();
    // reserve 3 segment per segment
    oWayFragmentArray.reserve( mSegmentArray.size() * 3 );

    if( sectionLinkInfo )
    {
        FOdysseyVectorVertex* currentVertex = firstVertex;

        currentVertex->SetID( oWayPointArray.size() );

        erasureFlag = GetErasureFlags( prevSectionLinkInfo ? prevSectionLinkInfo->section : nullptr
                                     , currentVertex
                                     , sectionLinkInfo->section );

        if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
        {
            hasHit = true;
        }

        oWayPointArray.emplace_back( currentVertex, erasureFlag | FWayPoint::Original );

        while( sectionLinkInfo )
        {
            FOdysseyVectorSection* section = sectionLinkInfo->section;
            FOdysseyVectorSegment* segment = section->GetSegment();
            double vertex0Radius = segment->GetVertex(0)->GetRadius();
            double vertex1Radius = segment->GetVertex(1)->GetRadius();
            uint32 sectionNextVertexIndex = ( sectionLinkInfo->sectionVertexIndex == 0 ) ? 1 : 0;
            FOdysseyVectorVertex* sectionNextVertex = section->GetVertex( sectionNextVertexIndex );
            FSectionLinkInfo* nextSectionLinkInfo = GetNextSectionLinkInfo( section
                                                                          , sectionNextVertex
                                                                          , sectionNextVertexIndex );
            FOdysseyVectorSection* nextSection = nextSectionLinkInfo ? nextSectionLinkInfo->section : nullptr;

            //section->Print();
            //if( nextSection ) nextSection->Print();
            //UE_LOG(LogTemp, Warning, TEXT("-------------"));

            erasureFlag = GetErasureFlags( section
                                         , sectionNextVertex
                                         , nextSection );

            if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
            {
                hasHit = true;
            }

            if( sectionNextVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                // for indexing
                sectionNextVertex->SetID( oWayPointArray.size() );

                oWayPointArray.emplace_back( sectionNextVertex, erasureFlag | FWayPoint::Original );

                // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                oWayFragmentArray.emplace_back( segment
                                              , oWayPointArray
                                              , currentVertex->GetID()
                                              , sectionNextVertex->GetID() );

                currentVertex = sectionNextVertex;
            }

            if( sectionNextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(sectionNextVertex);

                if( ( erasureFlag & FWayPoint::EntersErasureArea )
                  ||( erasureFlag & FWayPoint::LeavesErasureArea ) )
                {
                    double t = section->GetSegmentT( sectionNextVertexIndex );
                    double radius = ( t * vertex1Radius ) + ( ( 1.0f - t ) * vertex0Radius );
                    ::ULIS::FVec2D& vertexCoords = sectionNextVertex->GetCoords();
                    // owner is the paintgroup
                    BLMatrix2D& ownerWorldMatrix = sectionNextVertex->GetOwner()->GetWorldMatrix();
                    BLPoint vertexWorldCoords = ownerWorldMatrix.mapPoint( vertexCoords.x
                                                                         , vertexCoords.y );
                    BLPoint vertexPathCoords  = pathInverseWorldMatrix.mapPoint( vertexWorldCoords.x
                                                                               , vertexWorldCoords.y );
                    FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( vertexPathCoords.x
                                                                              , vertexPathCoords.y
                                                                              , radius );

                    // for indexing
                    newVertex->SetID( oWayPointArray.size() );

                    oWayPointArray.emplace_back( newVertex
                                               , intersectionVertex
                                               , erasureFlag
                                               , t );

                    // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                    oWayFragmentArray.emplace_back( segment
                                                  , oWayPointArray
                                                  , currentVertex->GetID()
                                                  , newVertex->GetID() );

                    //sectionNextVertex->SetID( newVertex->GetID() );

                    currentVertex = newVertex;
                }
            }
                              // loop prevention
            sectionLinkInfo = ( nextSection == firstSectionLinkInfo->section ) ? nullptr 
                                                                               : nextSectionLinkInfo;
        }
    }



#ifdef unused

    //for( FOdysseyVectorVertex* vertex : mVertexArray )
    {
        std::vector<FOdysseyVectorSection*> sectionArray;
        FOdysseyVectorVertex* vertex = mVertexArray[0];
        uint32 wayPointCount = oWayPointArray.size();

        GetSections( vertex, mPath, sectionArray );

        erasureFlag = GetErasureFlags( vertex, sectionArray, erasureFlag ) | FWayPoint::Original;

        // create a waypoint in any case
        oWayPointArray.emplace_back( vertex, erasureFlag );

        if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
        {
            hasHit = true;
        }
        // for indexing
        vertex->SetID( wayPointCount );
    }

    IterateSegments( [ this
                     , &erasureFlag
                     , &pathInverseWorldMatrix
                     , &oWayPointArray
                     , &oWayFragmentArray
                     , &hasHit ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
    {
        FOdysseyVectorVertex* currentVertex = vertex;
        FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( vertex );
        double vertex0Radius = segment->GetVertex(0)->GetRadius();
        double vertex1Radius = segment->GetVertex(1)->GetRadius();
        uint32 hitCount = 0;

        for( FOdysseyVectorIntersection* intersection : segment->GetIntersectionList() )
        {
            FOdysseyVectorVertexIntersection* intersectionVertex = intersection->GetIntersectionVertex();
            std::vector<FOdysseyVectorSection*> sectionArray;

            GetSections( intersectionVertex, segment, sectionArray );

            erasureFlag = GetErasureFlags( intersectionVertex, sectionArray, erasureFlag );

            if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
            {
                FOdysseyVectorVertex* isxVertex;

                double t = intersection->GetSegmentT();
                double radius = ( t * vertex1Radius ) + ( ( 1.0f - t ) * vertex0Radius );
                ::ULIS::FVec2D& vertexCoords = intersectionVertex->GetCoords();
                // owner is the paintgroup
                BLMatrix2D& ownerWorldMatrix = intersectionVertex->GetOwner()->GetWorldMatrix();
                BLPoint vertexWorldCoords = ownerWorldMatrix.mapPoint( vertexCoords.x
                                                                        , vertexCoords.y );
                BLPoint vertexPathCoords  = pathInverseWorldMatrix.mapPoint( vertexWorldCoords.x
                                                                            , vertexWorldCoords.y );
                isxVertex = new FOdysseyVectorVertex( vertexPathCoords.x
                                                    , vertexPathCoords.y
                                                    , radius );

                isxVertex->SetID( oWayPointArray.size() );
                // we also set it here because of the above "FWayPoint* chainedWayPoint = &oWayPointArray[sectionVertex->GetID()];"
                //intersectionVertex->SetID( oWayPointArray.size() );

                oWayPointArray.emplace_back( isxVertex
                                            , intersectionVertex
                                            , erasureFlag
                                            , t );

                // Note: wayFragments use waypoints ID because the array might grow (thus the pointer would change)
                oWayFragmentArray.emplace_back( segment
                                                , oWayPointArray
                                                , currentVertex->GetID()
                                                , isxVertex->GetID() );

                hasHit = true;

                currentVertex = isxVertex;
            }
        }

        std::vector<FOdysseyVectorSection*> sectionArray;

        GetSections( nextVertex, mPath, sectionArray );

        erasureFlag = GetErasureFlags( nextVertex, sectionArray, erasureFlag ) | FWayPoint::Original;

        // set index in the array
        nextVertex->SetID( oWayPointArray.size() );

        oWayPointArray.emplace_back( nextVertex, erasureFlag );

        if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
        {
            hasHit = true;
        }

        // create sub-segment
        oWayFragmentArray.emplace_back( segment
                                        , oWayPointArray
                                        , currentVertex->GetID()
                                        , nextVertex->GetID() );

        return false; // keep iterating;
    } );

    //---------------- parse hits if any ---------------------//
    if( oWayPointArray.size() > mVertexArray.size() )
    {
        hasHit = true;
    }

#endif

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

                FWayPoint wayPoint = FWayPoint( newVertex, nullptr, ( alphaValue == 0 ) ?                                                       FWayPoint::LeavesErasureArea
                                               : FWayPoint::EntersErasureArea, t );
                
                oWayPointArray.push_back( wayPoint );

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

                FWayPoint wayPoint = FWayPoint( newVertex, nullptr, ( alphaValue == 0 ) ?                                                       FWayPoint::LeavesErasureArea
                                               : FWayPoint::EntersErasureArea, t );

                oWayPointArray.push_back( wayPoint );

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
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
        bool revert = ( vertex == segment->GetVertex(0) ) ? false : true;
        FWayPoint* chainedWayPoint = &oWayPointArray[vertex->GetID()];
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
                                         , vertex1->GetID() );
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
                                         , vertex0->GetID() );
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
