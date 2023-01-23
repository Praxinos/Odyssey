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

void
UOdysseyVectorGroupPaint::CreateCycle( std::vector<UOdysseyVectorVertex*>& iNodeArray
                                     , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    int cycleSize = iNodeArray.size();

    UE_LOG(LogTemp, Warning, TEXT("cycle size %d"), cycleSize );

    for( int i = 0; i < cycleSize; i++ )
    {
        int p = ( ( i - 1 ) + cycleSize ) % cycleSize;

        iEdgeArray[i]->Block( iNodeArray[i] );
        iEdgeArray[p]->Block( iNodeArray[i] );
    }

    mLoopList.push_back( new FOdysseyVectorLoop( *this, iNodeArray, iEdgeArray ) );
}

// Ray casting algorithm. Draw a virtual ray from outside the cycle to the point.
// Count how many times it hit one of the cycle's edge. If that number is odd, it's inside.
// otherwise it's outside.
// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
static bool
InsideCycle( ::ULIS::FVec2D& iFromCoord, UOdysseyVectorVertex& iNode, std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
   ::ULIS::FVec2D& nodeCoord = iNode.GetCoords();
    uint32 hitCount = 0;

    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        FOdysseyVectorSection *edge = iEdgeArray[i];

        if( FOdysseyVector::IntersectSegment( iFromCoord
                                            , nodeCoord
                                            , edge->GetVertex(0)->GetCoords()
                                            , edge->GetVertex(1)->GetCoords()
                                            , nullptr
                                            , nullptr ) == true )
        {
            hitCount++;
        }
    }
UE_LOG(LogTemp, Warning, TEXT("hitCount %d"), hitCount );
    return ( hitCount % 2 ) ? true : false;
}

// a cycle is ChordLess if there is a point lying inside the cycle hasn't been visited.
// or if there is an edge connecting to points lying inside the circle that has not been visited.
static bool
IsChordless( ::ULIS::FVec2D& iFromCoord
           , std::vector<UOdysseyVectorVertex*>& iNodeArray
           , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    for( int i = 0; i < iNodeArray.size(); i++ )
    {
        UOdysseyVectorVertex* node = iNodeArray[i];
        std::list<FOdysseyVectorSection*>& nodeSectionList = node->GetSectionList();

        // check point inside
        for( std::list<FOdysseyVectorSection*>::iterator sit = nodeSectionList.begin(); sit != nodeSectionList.end(); ++sit )
        {
            FOdysseyVectorSection *edge = (*sit);
            UOdysseyVectorVertex* neighbour = ( edge->GetVertex(0) == node ) ? edge->GetVertex(1) : edge->GetVertex(0);

            if( edge->IsVisited() == false )
            {
                if( ( neighbour->IsVisited() == true ) )
                {
                    return false;
                }
                else
                {
                    if( InsideCycle( iFromCoord, *neighbour, iEdgeArray ) == true )
                    {
                        return false;
                    }
                }
            }
        }
    }
UE_LOG(LogTemp, Warning, TEXT("no hit %d") );
    return true;
}

void
UOdysseyVectorGroupPaint::March( UOdysseyVectorVertex* iNode
                               , std::vector<UOdysseyVectorVertex*>& iNodeArray
                               , std::vector<FOdysseyVectorSection*>& iEdgeArray
                               , std::list<FOdysseyVectorSection*>& iSectionList )
{
    std::list<FOdysseyVectorSection*>& nodeSectionList = iNode->GetSectionList();
    uint32 nodeArraySize;
    static int depth = 0;
    static ::ULIS::FVec2D minCoord;
    ::ULIS::FVec2D& nodeCoord = iNode->GetCoords();
/*
   if( depth++ > 100 ) return;
*/
    // init minimum point, we'll need it for the ray casting algorithm in the InsideCycle() function.
    // Note: we need it to be outside the cycle, so we arbitrarily substract 0.1f
    if( iNodeArray.size() == 0 )
    {
        minCoord.x = nodeCoord.x - 0.1f;
        minCoord.y = nodeCoord.y - 0.1f;
    }
    else
    {
        if( nodeCoord.x < minCoord.x ) minCoord.x = nodeCoord.x - 0.1f;
        if( nodeCoord.y < minCoord.y ) minCoord.y = nodeCoord.y - 0.1f;
    }

    iNode->SetVisited( true );
    iNodeArray.push_back( iNode );

    nodeArraySize = iNodeArray.size();
UE_LOG(LogTemp, Warning, TEXT("enter node %d %d - x:%f, y:%f"), iNode, nodeSectionList.size(), nodeCoord.x, nodeCoord.y );
    for( std::list<FOdysseyVectorSection*>::iterator sit = nodeSectionList.begin(); sit != nodeSectionList.end(); ++sit )
    {
        FOdysseyVectorSection *edge = (*sit);
        FOdysseyVectorSection *previousEdge = iEdgeArray.size() ? iEdgeArray.back() : nullptr;
 
        if( edge->IsVisited() == false )
        {
            edge->SetVisited( true );

            if( edge->IsBlocked( iNode ) == false )
            {
                UOdysseyVectorVertex* nextNode = ( edge->GetVertex(0) == iNode ) ? edge->GetVertex(1) : edge->GetVertex(0);

                iEdgeArray.push_back( edge );

                // then that's a cycle, baby.
                if( nextNode == iNodeArray[0] )
                {
UE_LOG(LogTemp, Warning, TEXT("candidate cycle") );
                    if( IsChordless( minCoord, iNodeArray, iEdgeArray ) == true )
                    {
                        // Create the cycle if it's chordless. Note, this function call also block leaving-edges and entering-edges.
                        CreateCycle( iNodeArray, iEdgeArray );
  
                        UE_LOG(LogTemp, Warning, TEXT("new cycle"));
                    }
                }
                else
                {
                    if( nextNode->IsVisited() == false )
                    {
                        March( nextNode, iNodeArray, iEdgeArray, iSectionList );
                    }
                }

                iEdgeArray.pop_back();
            }

            edge->SetVisited( false );
        }
    }

    iNodeArray.pop_back( );
    iNode->SetVisited( false );
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
    UE_LOG( LogTemp, Warning, TEXT("Simplify") );
    //UE_LOG( LogTemp, Warning, TEXT("Sections:%d"), sectionList.size() );
        iSectionList.remove_if( [&keepSimplifying]( FOdysseyVectorSection *section )
            {
                if( ( section->GetVertex(0)->GetSectionCount() == 1 )
                ||  ( section->GetVertex(1)->GetSectionCount() == 1 ) )
                {
                    keepSimplifying = true;
/*
                    section->GetVertex(0)->RemoveSection( section );
                    section->GetVertex(1)->RemoveSection( section );
*/
                    section->GetSegment()->RemoveSection( section );

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

void
UOdysseyVectorGroupPaint::FindCycles()
{
    std::list<UOdysseyVectorVertexIntersection*> intersectionVertexList;
    std::list<FOdysseyVectorSection*> sectionList;

    // TODO: clear mLoopList

    BuildGraph( intersectionVertexList, sectionList );

/* testing */
    for( std::list<UOdysseyVectorVertexIntersection*>::iterator sit = intersectionVertexList.begin(); sit != intersectionVertexList.end(); ++sit )
    {
        UOdysseyVectorVertexIntersection *node = Cast<UOdysseyVectorVertexIntersection>(*sit);

        UE_LOG(LogTemp,Warning,TEXT("DIAG Vertex:%d - sections:%d"), node, node->GetSectionCount());
    }
/**/



    while( intersectionVertexList.size() )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = intersectionVertexList.back();

        UE_LOG(LogTemp,Warning,TEXT("Vertex:%d - sections:%d"),intersectionVertex,intersectionVertex->GetSectionCount());

        std::vector<UOdysseyVectorVertex*> nodeArray;
        std::vector<FOdysseyVectorSection*> edgeArray;

        March( intersectionVertex, nodeArray, edgeArray, sectionList );

        intersectionVertexList.pop_back();
    }
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::CopyShape()
{
    return NewObject<UOdysseyVectorGroupPaint>();
}
