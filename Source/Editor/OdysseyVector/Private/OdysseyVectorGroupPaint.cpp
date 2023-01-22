#include "OdysseyVectorGroupPaint.h"

void
UOdysseyVectorGroupPaint::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorGroupPaint::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( std::list<FOdysseyVectorLoop*>::iterator lit = mLoopList.begin(); lit != mLoopList.end(); ++lit )
    {
        FOdysseyVectorLoop *loop = static_cast<FOdysseyVectorLoop*>(*lit);

        loop->Draw( iRoi, iFlags );
    }
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
/*
    if ( ( iX > mBBox.x ) && ( iX < ( mBBox.x + mBBox.w ) )
      && ( iY > mBBox.y ) && ( iY < ( mBBox.y + mBBox.h ) ) )
    {
        return this;
    }
*/
    return nullptr;
}

uint32
UOdysseyVectorGroupPaint::IntersectSegment( UOdysseyVectorSegmentCubic& iCubicSegment
                                          , std::list<UOdysseyVectorSegment*>& cubicSegmenList
                                          , std::list<UOdysseyVectorVertexIntersection*>& intersectionVertexList )
{
    uint32 intersectionCount = 0;

    for( std::list<UOdysseyVectorSegment*>::iterator sit = cubicSegmenList.begin(); sit != cubicSegmenList.end(); ++sit )
    {
        UOdysseyVectorSegmentCubic *intersectSegment = Cast<UOdysseyVectorSegmentCubic>(*sit);

        intersectionCount += iCubicSegment.Intersect( *intersectSegment, intersectionVertexList );
    }

    return intersectionCount;
}

static bool
HasChord( UOdysseyVectorVertex* iNode
        , UOdysseyVectorVertex* triangleNode
        , std::list<FOdysseyVectorSection*>& iSectionList )
{
    ::ULIS::FVec2D& line0p0 = triangleNode->GetCoords();
    ::ULIS::FVec2D& line0p1 =        iNode->GetCoords();

    for( std::list<FOdysseyVectorSection*>::iterator sit = iSectionList.begin(); sit != iSectionList.end(); ++sit )
    {
        FOdysseyVectorSection *section = (*sit);

        if( FOdysseyVector::IntersectSegment ( line0p0
                                             , line0p1
                                             , section->GetVertex(0)->GetCoords()
                                             , section->GetVertex(1)->GetCoords()
                                             , nullptr
                                             , nullptr ) )
        {
            return true;
        }
    }

    return false;
}

void
UOdysseyVectorGroupPaint::CreateCycle( std::vector<UOdysseyVectorVertex*>& iNodeArray
                                     , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    int cycleSize = iNodeArray.size();

    for( int i = 0; i < cycleSize; i++ )
    {
        int p = ( ( i - 1 ) + cycleSize ) % cycleSize;

        iEdgeArray[i]->Block( iNodeArray[i] );
        iEdgeArray[p]->Block( iNodeArray[i] );
    }

    mLoopList.push_back( new FOdysseyVectorLoop( *this, iNodeArray, iEdgeArray ) );
}

void
UOdysseyVectorGroupPaint::March( UOdysseyVectorVertex* iNode
                               , std::vector<UOdysseyVectorVertex*>& iNodeArray
                               , std::vector<FOdysseyVectorSection*>& iEdgeArray
                               , std::list<FOdysseyVectorSection*>& iSectionList )
{
    std::list<FOdysseyVectorSection*>& nodeSectionList = iNode->GetSectionList();
    uint32 nodeArraySize;

    iNodeArray.push_back( iNode );

    nodeArraySize = iNodeArray.size();

    for( std::list<FOdysseyVectorSection*>::iterator sit = nodeSectionList.begin(); sit != nodeSectionList.end(); ++sit )
    {
        FOdysseyVectorSection *edge = (*sit);
        FOdysseyVectorSection *previousEdge = iEdgeArray.size() ? iEdgeArray.back() : nullptr;
 

        if( edge->IsBlocked( iNode ) == false )
        {
UE_LOG(LogTemp, Warning, TEXT("enter"));
            if( edge != previousEdge  )
            {
                UOdysseyVectorVertex* nextNode = ( edge->GetVertex(0) == iNode ) ? edge->GetVertex(1) : edge->GetVertex(0);

                if( nextNode == iNodeArray[0] )
                {
                    // then that's a cycle, baby. Note, this function call also block leaving-edges.
                    iEdgeArray.push_back( edge );
                    CreateCycle( iNodeArray, iEdgeArray );
                    iEdgeArray.pop_back();
UE_LOG(LogTemp, Warning, TEXT("new cycle"));
                }
                else
                {
                    if( nodeArraySize >= 2 )
                    {
                        UOdysseyVectorVertex* triangleRootNode = iNodeArray[nodeArraySize - 2];

                        if( HasChord( nextNode, triangleRootNode, iSectionList ) )
                        {
                            continue;
                        }
                    }

                    iEdgeArray.push_back( edge );
                    March( nextNode, iNodeArray, iEdgeArray, iSectionList );
                    iEdgeArray.pop_back();
                }
            }
        }
    }

    iNodeArray.pop_back( );
}

void
UOdysseyVectorGroupPaint::FindCycles()
{
    std::list<UOdysseyVectorVertexIntersection*> intersectionVertexList;
    std::list<FOdysseyVectorSection*> sectionList;

    // TODO: clear mLoopList

    BuildGraph( intersectionVertexList, sectionList );

    if( intersectionVertexList.size() )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = intersectionVertexList.back();

        while( intersectionVertexList.size() )
        {
            std::vector<UOdysseyVectorVertex*> nodeArray;
            std::vector<FOdysseyVectorSection*> edgeArray;

            March( intersectionVertex, nodeArray, edgeArray, sectionList );

            intersectionVertexList.pop_back();

            intersectionVertex = intersectionVertexList.back();
        }
    }
}

void
UOdysseyVectorGroupPaint::SimplifyGraph( std::list<FOdysseyVectorSection*>& iSectionList )
{
    bool keepSimplifying;

    for( std::list<UOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        UOdysseyVectorObject *child = (*oit);

        if( child->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
        {
            UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(child);
            std::list<UOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();

            for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
            {
                UOdysseyVectorSegmentCubic *cubicSegment = Cast<UOdysseyVectorSegmentCubic>(*it);
                std::list<FOdysseyVectorSection*>& segmentSectionList = cubicSegment->GetSectionList();

                iSectionList.insert( iSectionList.end(), segmentSectionList.begin(), segmentSectionList.end() );
            }
        }
    }

    do
    {
        keepSimplifying = false;
    //UE_LOG( LogTemp, Warning, TEXT("Sections:%d"), sectionList.size() );
        iSectionList.remove_if( [&keepSimplifying]( FOdysseyVectorSection *section )
            {
                if( ( section->GetVertex(0)->GetSectionCount() == 1 )
                ||  ( section->GetVertex(1)->GetSectionCount() == 1 ) )
                {
                    keepSimplifying = true;

                    section->GetVertex(0)->RemoveSection( section );
                    section->GetVertex(1)->RemoveSection( section );

                    return true;
                }

                return false;
            } );
    } while ( keepSimplifying );
    //UE_LOG( LogTemp, Warning, TEXT("End Sections:%d"), sectionList.size() );
}

void
UOdysseyVectorGroupPaint::BuildGraph( std::list<UOdysseyVectorVertexIntersection*>& iIntersectionVertexList
                                    , std::list<FOdysseyVectorSection*>& iSectionList )
{
    std::list<UOdysseyVectorSegment*> cubicSegmenList;
    UOdysseyVectorSegmentCubic *cubicSegment;
    uint32 intersectionCount = 0;

    for( std::list<UOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        UOdysseyVectorObject *child = (*oit);

        if( child->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
        {
            UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(child);
            std::list<UOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();

            for( std::list<UOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                cubicSegment = Cast<UOdysseyVectorSegmentCubic>(*sit);

                cubicSegment->ClearIntersections();

                cubicSegmenList.push_back( cubicSegment );
            }
        }
    }

    cubicSegment = cubicSegmenList.size() ? Cast<UOdysseyVectorSegmentCubic>( cubicSegmenList.back() ) : nullptr;

    while( cubicSegment )
    {
        intersectionCount += IntersectSegment ( *cubicSegment, cubicSegmenList, iIntersectionVertexList );

        // remove segment from list as they are tested
        cubicSegmenList.pop_back();

        cubicSegment = cubicSegmenList.size() ? Cast<UOdysseyVectorSegmentCubic>( cubicSegmenList.back() ) : nullptr;
    }

    // Step Two - simply the graph by removing sections that are not part of a cycle. to do that we proceed with several passes
    // by removing the sections that are at a dead-end and go-on until no section is a dead-end.

    SimplifyGraph( iSectionList );

    UE_LOG( LogTemp, Warning, TEXT("Intersections:%d"), iIntersectionVertexList.size() );
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::CopyShape()
{
    return NewObject<UOdysseyVectorGroupPaint>();
}
