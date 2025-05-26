// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorChain.h"
#include "OdysseyVector.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexIntersection.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSection.h"
#include "OdysseyVectorSegmentCubic.h"

FWayFragment::FWayFragment( FOdysseyVectorSegment* iSegment
                          , FWayPoint* iWayPoint0
                          , FWayPoint* iWayPoint1
                          , bool iErased )
{
    double t0, t1;

    segment = iSegment;

    wayPoint0 = iWayPoint0;
    wayPoint1 = iWayPoint1;

    wayPoint0->fragmentArray.push_back( this );
    wayPoint1->fragmentArray.push_back( this );

    erased = iErased;

    if( wayPoint0->flags & FWayPoint::Original )
    {
        t0 = wayPoint0->vertex->GetIndex( iSegment );
    }
    else
    {
        t0 = wayPoint0->t;
    }

    if( wayPoint1->flags & FWayPoint::Original )
    {
        t1 = wayPoint1->vertex->GetIndex( iSegment );
    }
    else
    {
        t1 = wayPoint1->t;
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

FWayFragment*
FWayFragment::GetNext()
{
    for( FWayFragment* candidateFragment : wayPoint1->fragmentArray )
    {
        if( candidateFragment->wayPoint0 == wayPoint1 )
        {
            return candidateFragment;
        }
    }

    return nullptr;
}

FWayFragment*
FWayFragment::GetPrev()
{
    for( FWayFragment* candidateFragment : wayPoint0->fragmentArray )
    {
        if( candidateFragment->wayPoint1 == wayPoint0 )
        {
            return candidateFragment;
        }
    }

    return nullptr;
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
            if( sectionLinkInfo.section->IsErased() == false )
            {
                if( sectionLinkInfo.section != iLastSectionInfo->section )
                {
                    oSectionLinkInfoArray.push_back( &sectionLinkInfo );
                }
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

static uint32
GetErasureFlags( FOdysseyVectorVertex* iVertex
               , eVertexSectionTypeQuery iQuery
               , FOdysseyVectorObject* iOwner
               , FOdysseyVectorSegment* iSegment )
{
    uint32 erasedSectionCount;
    uint32 sectionCount;

    erasedSectionCount = iVertex->GetErasedSectionCount( iQuery, iOwner, iSegment );
    sectionCount = iVertex->GetSectionCount( iQuery, iOwner, iSegment );

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
FOdysseyVectorChain::EraseSections( std::vector<FWayPoint>& oWayPointBuffer
                                  , std::vector<FWayFragment>& oWayFragmentBuffer )
{
    BLMatrix2D& pathInverseWorldMatrix = mPath->GetInverseWorldMatrix();
    std::vector<FOdysseyVectorSection*> pickedSectionArray;
    FOdysseyVectorVertex* firstVertex = mVertexArray[0];
    FOdysseyVectorSegment* firstSegment = mSegmentArray[0];
    FOdysseyVectorVertex* currentVertex = firstVertex;
    FOdysseyVectorVertex* chainedVertex = firstVertex;
    uint32 waypointReserveCount = 1;
    bool hasHit = false;

    for( FOdysseyVectorSegment* segment : mSegmentArray )
    {
        waypointReserveCount += segment->GetSectionCount();
    }

    oWayPointBuffer.clear();
    // reserve 1 point per vertex + 2 point per segment
    oWayPointBuffer.reserve( waypointReserveCount );

    oWayFragmentBuffer.clear();
    // reserve 3 segment per segment
    oWayFragmentBuffer.reserve( mSegmentArray.size() * 3 );

    for( FOdysseyVectorVertex* vertex : mVertexArray )
    {
        uint32 erasureFlag = GetErasureFlags( vertex
                                            , eVertexSectionTypeQuery::SameOwner
                                            , mPath
                                            , nullptr );
        uint32 vertexID = oWayPointBuffer.size();

        if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
        {
            hasHit = true;
        }

        oWayPointBuffer.emplace_back( vertex, erasureFlag | FWayPoint::Original );

        vertex->SetID( vertexID );
    }

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
            FWayPoint* chainedWayPoint = &oWayPointBuffer.back();

            if( nextVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                oWayFragmentBuffer.emplace_back( segment
                                               , &oWayPointBuffer[chainedVertex->GetID()]
                                               , &oWayPointBuffer[nextVertex->GetID()]
                                               , section->IsErased() );

                chainedVertex = nextVertex;
            }

            if( nextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
            {
                FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(nextVertex);

                uint32 erasureFlag = GetErasureFlags( nextVertex
                                                    , eVertexSectionTypeQuery::SameSegment
                                                    , nullptr
                                                    , segment );

                if( ( erasureFlag & FWayPoint::OutsideErasureArea ) == 0 )
                {
                    hasHit = true;
                }

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
                    uint32 vertexID = oWayPointBuffer.size();

                    oWayPointBuffer.emplace_back( derivedVertex
                                                , intersectionVertex
                                                , erasureFlag
                                                , t );

                    intersectionVertex->SetID( vertexID );

                    oWayFragmentBuffer.emplace_back( segment
                                                   , &oWayPointBuffer[chainedVertex->GetID()]
                                                   , &oWayPointBuffer[nextVertex->GetID()]
                                                   , section->IsErased() );

                    chainedVertex = nextVertex;
                }
            }

            currentVertex = nextVertex;
        }
    }

    return hasHit;
}

//static
FWayFragment*
FOdysseyVectorChain::GetStartFragment( FWayFragment* iFragment )
{
    FWayFragment* currFragment = iFragment;

    // extend prev
    while( currFragment )
    {
        FWayFragment* prevFragment = currFragment->GetPrev();

        if( ( prevFragment == nullptr )
         || ( prevFragment == iFragment ) // loop prevention
         || ( ( prevFragment->erased == true ) &&  ( currFragment->erased == false ) ) )
        {
            return currFragment;
        }

        currFragment = prevFragment;
    }

    return nullptr; // this case should never be met anyways. theorically
}

// static
eSegmentAdditionFlags
FOdysseyVectorChain::SegmentAdditionPolicy( FWayFragment* iFragment, bool iSplit )
{
    eSegmentAdditionFlags retFlags = eSegmentAdditionFlags::RemoveOriginalSegment;
    FWayFragment* prevFragment = iFragment->GetPrev();

    if( iSplit )
    {
        if( ( iFragment->erased == true ) && ( ( prevFragment == nullptr )
                                            || ( prevFragment->erased == false ) ) )
        {
            retFlags |= eSegmentAdditionFlags::CreateNewPath;
        }
    }

    if( iFragment->erased == false )
    {
        retFlags |= eSegmentAdditionFlags::CreateDerivedSegment;
    }

    if( iFragment->erased == true )
    {
        retFlags |= eSegmentAdditionFlags::RemoveOriginalSegment;
    }

    return retFlags;
}

// static
eVertexAdditionFlags
FOdysseyVectorChain::VertexAdditionPolicy( FWayPoint* iWayPoint, bool iSplit )
{
    eVertexAdditionFlags retFlags = eVertexAdditionFlags::None;

    if( iSplit )
    {
        if( iWayPoint->flags & FWayPoint::Original )
        {
            retFlags |= ( eVertexAdditionFlags::RemoveOriginalVertex );

            if( iWayPoint->flags & FWayPoint::BordersErasureArea )
            {
                if( iWayPoint->vertex->GetSegmentCount() == 2 )
                {
                    retFlags |= ( eVertexAdditionFlags::CreateDerivedVertex );
                }
            }

            if( iWayPoint->flags & FWayPoint::OutsideErasureArea )
            {
                retFlags |= ( eVertexAdditionFlags::CreateDerivedVertex );
            }
        }
    }

    if( iWayPoint->flags & FWayPoint::InsideErasureArea )
    {
        if( iWayPoint->flags & FWayPoint::Original )
        {
            retFlags |= eVertexAdditionFlags::RemoveOriginalVertex;
        }
    }

    if( iWayPoint->flags & FWayPoint::BordersErasureArea )
    {
        if( ( iWayPoint->flags & FWayPoint::Original ) == 0 )
        {
            retFlags |= eVertexAdditionFlags::CreateBoundaryVertex;
        }
    }

    return retFlags;
}

void
FOdysseyVectorChain::ParseWayPoints( std::vector<FWayPoint>& iWayPointBuffer
                                   , std::vector<FWayFragment>& iWayFragmentBuffer
                                   , std::vector<FOdysseyVectorObject*>& oAddedPathArray
                                   , std::vector<FOdysseyVectorVertex*>& oAddedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                                   , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                   , bool iSplit )
{
    uint32 addedSegmentCountBeforeAlter = oAddedSegmentArray.size();
    FOdysseyVectorPath* currentPath = mPath;
    bool needNewPath = false;

    if( iWayPointBuffer.size() )
    {
        FWayPoint& firstWayPoint = iWayPointBuffer[0];
        FWayFragment *firstFragment = &iWayFragmentBuffer[0];
        FWayFragment *startFragment = GetStartFragment( firstFragment );
        FWayFragment *currFragment = startFragment;

        // step 1 : create needed vertices
        for( FWayPoint& wayPoint : iWayPointBuffer )
        {
            eVertexAdditionFlags vertexAdditionFlags = VertexAdditionPolicy( &wayPoint, iSplit );

            if( ( vertexAdditionFlags & eVertexAdditionFlags::RemoveOriginalVertex ) == eVertexAdditionFlags::RemoveOriginalVertex )
            {
                // mark original vertex for deletion. No duplicates (duplicates happen in case of loops)
                if( std::find( oRemovedVertexArray.begin(), oRemovedVertexArray.end(), wayPoint.vertex ) == oRemovedVertexArray.end() )
                {
                    oRemovedVertexArray.push_back( wayPoint.vertex );
                }
            }

            // boundary vertices are guaranteed unique per nature, no need to check uniqueness
            if( ( vertexAdditionFlags & eVertexAdditionFlags::CreateBoundaryVertex ) == eVertexAdditionFlags::CreateBoundaryVertex )
            {
                oAddedVertexArray.push_back( wayPoint.vertex );
            }

            if( ( vertexAdditionFlags & eVertexAdditionFlags::CreateDerivedVertex ) == eVertexAdditionFlags::CreateDerivedVertex )
            {
                ::ULIS::FVec2D& coords = wayPoint.vertex->GetCoords();
                double radius = wayPoint.vertex->GetRadius();
                bool handleAligned = wayPoint.vertex->IsHandleAligned();

                // replaces the current original vertex. The latter is already saved in oRemovedVertexArray
                wayPoint.vertex = new FOdysseyVectorVertex( coords.x, coords.y, radius );
                wayPoint.vertex->SetHandleAligned( handleAligned );

                // a new vertex for each former vertex
                oAddedVertexArray.push_back( wayPoint.vertex );
            }
        }

        while( currFragment )
        {
            FWayPoint* wayPoint0 = currFragment->wayPoint0;
            FWayPoint* wayPoint1 = currFragment->wayPoint1;
            eSegmentAdditionFlags segmentAdditionFlags = SegmentAdditionPolicy( currFragment, iSplit );
            FWayFragment* nextFragment = currFragment->GetNext();

            //UE_LOG(LogTemp, Warning, TEXT("fragment : %d:%x %d:%x"), wayPoint0->vertex->GetID(), wayPoint0->flags
            //                                                       , wayPoint1->vertex->GetID(), wayPoint1->flags );


            if( ( segmentAdditionFlags & eSegmentAdditionFlags::RemoveOriginalSegment ) == eSegmentAdditionFlags::RemoveOriginalSegment )
            {
                // mark original segment for deletion. No duplicates
                if( std::find( oRemovedSegmentArray.begin(), oRemovedSegmentArray.end(), currFragment->segment ) == oRemovedSegmentArray.end() )
                {
                    oRemovedSegmentArray.push_back( currFragment->segment );
                }

                if( ( segmentAdditionFlags & eSegmentAdditionFlags::CreateNewPath ) == eSegmentAdditionFlags::CreateNewPath )
                {
                    needNewPath = true;
                }
            }

            if( ( segmentAdditionFlags & eSegmentAdditionFlags::CreateDerivedSegment ) == eSegmentAdditionFlags::CreateDerivedSegment )
            {
                if( needNewPath )
                {
                    currentPath = new FOdysseyVectorPath( mPath->GetName() );
                    // for postprocessing. the path is not added to the parent yet
                    currentPath->SetParent( mPath->GetParent() );
                    mPath->ExportParam( currentPath, true );
                    mPath->CopyTransformation( *currentPath );

                    oAddedPathArray.push_back( currentPath );

                    needNewPath = false;
                }

                FOdysseyVectorVertex* destVertex0 = wayPoint0->vertex;
                FOdysseyVectorVertex* destVertex1 = wayPoint1->vertex;
                // currentPath can be null at first for some segments. This will be handled
                // in a second pass this is needed for segment chains that loop.
                FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( currentPath
                                                                                       , destVertex0
                                                                                       , currFragment->bezier[1].x
                                                                                       , currFragment->bezier[1].y
                                                                                       , currFragment->bezier[2].x
                                                                                       , currFragment->bezier[2].y
                                                                                       , destVertex1
                                                                                       , true );

                // store path here even if technically the vertex does not belong to the path yet.
                // it will once we call path->AddVertex() in the FOdysseyVectorPath::Erase() func.
                wayPoint0->vertex->SetOwner( currentPath );
                wayPoint1->vertex->SetOwner( currentPath );

                // mark new segment for addition
                oAddedSegmentArray.push_back( newSegment );
            }

            currFragment = ( nextFragment == startFragment ) ? nullptr : nextFragment;
        }
    }
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
FOdysseyVectorVertex*
FOdysseyVectorChain::TraceLine( int32 iX0
                              , int32 iY0
                              , double iT0
                              , double iRadius0
                              , int32 iX1
                              , int32 iY1
                              , double iT1
                              , double iRadius1
                              , BLImageData* iImageData
                              , std::vector<FWayPoint>& oWayPointBuffer
                              , std::vector<FMetaFragment>& oMetaFragmentBuffer
                              , FOdysseyVectorVertex* iChainVertex
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
    ::ULIS::FVec2D bezier[4];
    double lastT = iT0;

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            uint8 alphaValue = GetAlpha( x, y, iImageData );

            if( CheckContrast( alphaValue, lastAlphaValue ) )
            {
                double radius = ( t * iRadius1 ) + ( ( 1.0f - t ) * iRadius0 );
                // coords will always be right outside the erasure area
                ::ULIS::FVec2D newVertexAt = alphaValue ? iSegment->GetPointAt( lastT )
                                                        : iSegment->GetPointAt( t );
                FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( newVertexAt.x
                                                                          , newVertexAt.y
                                                                          , radius );
                uint32 waypointID = oWayPointBuffer.size();
                uint32 flag = FWayPoint::BordersErasureArea;
                oWayPointBuffer.emplace_back( newVertex
                                            , nullptr
                                            , flag
                                            , t );

                newVertex->SetID( waypointID );

                oMetaFragmentBuffer.emplace_back( iSegment
                                                , iChainVertex->GetID()
                                                , newVertex->GetID()
                                                , lastAlphaValue ? true : false );

                iChainVertex = newVertex;
            }

            lastAlphaValue = alphaValue;
            lastT = t;

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
                double radius = ( t * iRadius1 ) + ( ( 1.0f - t ) * iRadius0 );
                // coords will always be right outside the erasure area
                ::ULIS::FVec2D newVertexAt = alphaValue ? iSegment->GetPointAt( lastT )
                                                        : iSegment->GetPointAt( t );
                FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( newVertexAt.x
                                                                          , newVertexAt.y
                                                                          , radius );
                uint32 waypointID = oWayPointBuffer.size();
                uint32 flag = FWayPoint::BordersErasureArea;
                oWayPointBuffer.emplace_back( newVertex
                                            , nullptr
                                            , flag
                                            , t );

                newVertex->SetID( waypointID );

                oMetaFragmentBuffer.emplace_back( iSegment
                                                , iChainVertex->GetID()
                                                , newVertex->GetID()
                                                , lastAlphaValue ? true : false );

                iChainVertex = newVertex;
            }

            lastAlphaValue = alphaValue;
            lastT = t;

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

    return iChainVertex;
}

void
FOdysseyVectorChain::VertexToWaypoint( BLImageData* iImageData
                                     , FOdysseyVectorVertex* iVertex
                                     , std::vector<FWayPoint>& oWayPointBuffer )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    ::ULIS::FVec2D& coords = iVertex->GetCoords();
    BLPoint point = worldMatrix.mapPoint( coords.x, coords.y );
    uint8 alpha = GetAlpha( point.x, point.y, iImageData );

    if( alpha == 0 ) // vertex in dark zone, keep it
    {
        oWayPointBuffer.emplace_back( iVertex
                                    , FWayPoint::OutsideErasureArea
                                    | FWayPoint::Original );
    }
    else
    {
        oWayPointBuffer.emplace_back( iVertex
                                    , FWayPoint::InsideErasureArea
                                    | FWayPoint::Original );
    }
}

bool
FOdysseyVectorChain::EraseSegments( BLImageData* iImageData
                                  , std::vector<FWayPoint>& oWayPointBuffer
                                  , std::vector<FWayFragment>& oWayFragmentBuffer )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
    FOdysseyVectorVertex* firstVertex = mVertexArray[0];
    std::vector<FMetaFragment> metaFragmentBuffer;
    bool hasHit = false;

    oWayPointBuffer.clear();
    // reserve 1 point per vertex + 2 point per segment to limit reallocations (just for performance)
    oWayPointBuffer.reserve( mVertexArray.size() + ( mSegmentArray.size() * 2 ) );

    // reserve 3 meta segment per segment, just for performance
    metaFragmentBuffer.reserve( mSegmentArray.size() * 3 );

    for( uint32 i = 0; i < mVertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = mVertexArray[i];
        uint32 waypointID = oWayPointBuffer.size();

        VertexToWaypoint( iImageData, vertex, oWayPointBuffer );

        vertex->SetID( waypointID );
    }

    IterateSegments( [ this
                     , iImageData
                     , &oWayPointBuffer
                     , &metaFragmentBuffer
                     , &hasHit
                     , &worldMatrix ]( FOdysseyVectorVertex* vertex, FOdysseyVectorSegment* segment ) -> bool
    {
        // for better precision when the line has only few fractions. This is slow
        // and will be changed.
        segment->Update( FOdysseyVectorObject::UPDATE_NEEDPOLYLINE );

        std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();
        FOdysseyVectorVertex* otherVertex = segment->GetOtherVertex( vertex );
        FOdysseyVectorSegment* nextSegment = otherVertex->GetOtherSegment( segment );
        bool revert = ( vertex == segment->GetVertex(0) ) ? false : true;
        FOdysseyVectorVertex* chainVertex = vertex;

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

                chainVertex = TraceLine( p0.x
                                       , p0.y
                                       , fraction.fromT
                                       , vertex->GetRadius()
                                       , p1.x
                                       , p1.y
                                       , fraction.toT
                                       , otherVertex->GetRadius()
                                       , iImageData
                                       , oWayPointBuffer
                                       , metaFragmentBuffer
                                       , chainVertex
                                       , segment
                                       , revert );
            }
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

                chainVertex = TraceLine( p1.x
                                       , p1.y
                                       , fraction.toT
                                       , vertex->GetRadius()
                                       , p0.x
                                       , p0.y
                                       , fraction.fromT
                                       , otherVertex->GetRadius()
                                       , iImageData
                                       , oWayPointBuffer
                                       , metaFragmentBuffer
                                       , chainVertex
                                       , segment
                                       , revert );
            }
        }

        // last fragment
        metaFragmentBuffer.emplace_back( segment
                                       , chainVertex->GetID()
                                       , otherVertex->GetID()
                                       , ( oWayPointBuffer[otherVertex->GetID()].flags & FWayPoint::InsideErasureArea ) ? true : false );

        return false; // keep iterating;
    } );

    oWayFragmentBuffer.clear();
    oWayFragmentBuffer.reserve( metaFragmentBuffer.size() );

    // create fragments afterwards so that the waypoint pointers won't change due to array growing with push_backs
    for( FMetaFragment& metaFragment : metaFragmentBuffer )
    {
        FWayPoint* wayPoint0 = &oWayPointBuffer[metaFragment.wayPoint0Index];
        FWayPoint* wayPoint1 = &oWayPointBuffer[metaFragment.wayPoint1Index];

        oWayFragmentBuffer.emplace_back( metaFragment.segment
                                       , wayPoint0
                                       , wayPoint1
                                       , metaFragment.erased );

        if( ( wayPoint0->flags & FWayPoint::OutsideErasureArea ) == 0 )
        {
            hasHit = true;
        }

        if( ( wayPoint1->flags & FWayPoint::OutsideErasureArea ) == 0 )
        {
            hasHit = true;
        }
    }

    return hasHit;
}
