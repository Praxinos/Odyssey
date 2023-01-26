#include "OdysseyVectorGroupPaint.h"

void
UOdysseyVectorGroupPaint::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorGroupPaint::ApplyBucket( FOdysseyVectorBucket& iBucket )
{
    for( std::list<FOdysseyVectorLoop*>::iterator lit = mLoopList.begin(); lit != mLoopList.end(); ++lit )
    {
        FOdysseyVectorLoop *loop = static_cast<FOdysseyVectorLoop*>(*lit);

        if( loop->HitTest( iBucket.GetCoords().x, iBucket.GetCoords().y ) )
        {
            loop->SetColor( iBucket.GetColor() );
        }
    }

    /*Invalidate();*/
}

FOdysseyVectorBucket*
UOdysseyVectorGroupPaint::NewBucket( uint32 iColor, double iX, double iY )
{
    FOdysseyVectorBucket* bucket = new FOdysseyVectorBucket( *this, iColor, iX, iY );

    mBucketList.push_back( bucket );

    ApplyBucket( *bucket );

    return bucket;
}

void
UOdysseyVectorGroupPaint::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( std::list<FOdysseyVectorLoop*>::iterator lit = mLoopList.begin(); lit != mLoopList.end(); ++lit )
    {
        FOdysseyVectorLoop *loop = static_cast<FOdysseyVectorLoop*>(*lit);

        loop->Draw( iRoi, iFlags );
    }

    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        bucket->Draw( iRoi, iFlags );
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
//UE_LOG(LogTemp, Warning, TEXT("hitCount %d"), hitCount );
    return ( hitCount % 2 ) ? true : false;
}

static void
MarkCycle( std::vector<UOdysseyVectorVertex*>& iVertexArray
         , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        iEdgeArray[i]->SetInCycle( true );
    }

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        iVertexArray[i]->SetInCycle( true );
    }
}

static void
UnmarkCycle( std::vector<UOdysseyVectorVertex*>& iVertexArray
           , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        iEdgeArray[i]->SetInCycle( false );
    }

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        iVertexArray[i]->SetInCycle( false );
    }
}

// a cycle is ChordLess if there is a point lying inside the cycle hasn't been visited.
// or if there is an edge connecting to points lying inside the circle that has not been visited.
static bool
IsChordless( ::ULIS::FVec2D& iFromCoord
           , std::vector<UOdysseyVectorVertex*>& iNodeArray
           , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    // Triangles are chordless per se.
    if( iNodeArray.size() <= 3 )
    {
        return true;
    }

    for( int i = 0; i < iNodeArray.size(); i++ )
    {
        UOdysseyVectorVertex* node = iNodeArray[i];
        std::list<FOdysseyVectorSection*>& nodeSectionList = node->GetSectionList();

        // check point inside
        for( std::list<FOdysseyVectorSection*>::iterator sit = nodeSectionList.begin(); sit != nodeSectionList.end(); ++sit )
        {
            FOdysseyVectorSection *edge = (*sit);
            UOdysseyVectorVertex* neighbour = ( edge->GetVertex(0) == node ) ? edge->GetVertex(1) : edge->GetVertex(0);

            if( edge->IsInCycle() == false )
            {
                if( ( neighbour->IsInCycle() == true ) )
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
//UE_LOG(LogTemp, Warning, TEXT("no hit %d") );
    return true;
}

static void
PrintNode( std::vector<UOdysseyVectorVertex*>& vertexArray
         ,std::vector<FOdysseyVectorSection*>& sectionArray)
{
    //UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d -- %d)"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), sectionArray[i]->GetVertex(1) );
    }
}

FOdysseyVectorSection* GetStartSection( FCycleNode* iNode )
{
    FOdysseyVectorSection* section = nullptr;

    while ( iNode->parent )
    {
        section = iNode->section;

        iNode = iNode->parent;
    }

    return section;
}

static ::ULIS::FVec2D
MergeNode( FCycleNode* iNode0
         , FOdysseyVectorSection* iEdge
         , std::list<FOdysseyVectorSection*>* iBypassEdgeList
         , FCycleNode* iNode1
         , std::vector<UOdysseyVectorVertex*>& vertexArray
         , std::vector<FOdysseyVectorSection*>& sectionArray )
{
    FCycleNode* mergeNode = iNode0;
    FOdysseyVectorSection* mergeSection = iEdge;
    std::list<FOdysseyVectorSection*>* mergeBypassEdgeList = iBypassEdgeList;
    uint32 node0Count = ( ( iNode0 ) ? iNode0->depth + 1 : 0 );
    uint32 node1Count = ( ( iNode1 ) ? iNode1->depth     : 0 ); // note: we don't add the last node
    uint32 nodeCount = node0Count + node1Count;
    ::ULIS::FVec2D minCoord = ::ULIS::FVec2D(0,0);

    if( nodeCount )
    {
        int vrank = 0;
        int srank = 0;

        minCoord = iNode0->vertex->GetCoords();

        vertexArray.resize( nodeCount );
        sectionArray.resize( nodeCount );

        // concatenate iNode1 and iNode0 except for the root node.
        while( iNode0 )
        {
            ::ULIS::FVec2D& nodeCoord = iNode0->vertex->GetCoords();
            int i = iNode0->depth;
            int p = ( i - 1 + nodeCount ) % nodeCount;

            vertexArray[i]  = iNode0->vertex;
            sectionArray[p] = iNode0->section;

            // find minimum point, we'll need it for the ray casting algorithm in the InsideCycle() function.
            // Note: we need it to be outside the cycle, so we arbitrarily substract 0.1f
            if( nodeCoord.x < minCoord.x ) minCoord.x = nodeCoord.x - 0.1f;
            if( nodeCoord.y < minCoord.y ) minCoord.y = nodeCoord.y - 0.1f;

            iNode0 = iNode0->parent;
        }

        while( iNode1->parent )
        {
            ::ULIS::FVec2D& nodeCoord = iNode1->vertex->GetCoords();
            int i = node0Count + node1Count - iNode1->depth;
            int n = ( i + 1 + nodeCount ) % nodeCount;

            vertexArray[i] = iNode1->vertex;
            sectionArray[i] = iNode1->section;

            //iEdge = iNode1->section;

            // find minimum point, we'll need it for the ray casting algorithm in the InsideCycle() function.
            // Note: we need it to be outside the cycle, so we arbitrarily substract 0.1f
            if( nodeCoord.x < minCoord.x ) minCoord.x = nodeCoord.x - 0.1f;
            if( nodeCoord.y < minCoord.y ) minCoord.y = nodeCoord.y - 0.1f;

            iNode1 = iNode1->parent;
        }

        sectionArray[mergeNode->depth] = iEdge;
    }

    PrintNode( vertexArray, sectionArray );

    return minCoord;
}

// 2 Nodes met at collision point.
bool
UOdysseyVectorGroupPaint::MakeCycle( ::ULIS::FVec2D& minCoord
                                   , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                   , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    uint64 cycleID = FOdysseyVectorLoop::GenerateID( iSectionArray );
    bool ret = false;

    if( FOdysseyVectorLoop::Exists( cycleID, iVertexArray, iSectionArray ) == false )
    {

        MarkCycle( iVertexArray, iSectionArray );
            UE_LOG(LogTemp, Warning, TEXT("TEST CHORDLESS"));

        if( IsChordless( minCoord, iVertexArray, iSectionArray ) == true )
        {
            int cycleSize = iSectionArray.size();
            FOdysseyVectorLoop* cycle;

            //UE_LOG(LogTemp, Warning, TEXT("cycle size %d"), cycleSize );

            // Block leaving edge. The vertex wont be able to exit from it anymore.
            for( int i = 0; i < cycleSize; i++ )
            {
                /*int p = ( i - 1 + cycleSize ) % cycleSize;*/

                iSectionArray[i]->Block( iVertexArray[i] );

                // clean flags
                /*iEdgeArray[i]->SetVisited( false );
                iNodeArray[i]->SetVisited( false );*/
            }

            cycle = new FOdysseyVectorLoop( *this, cycleID, iVertexArray, iSectionArray );

            mLoopList.push_back( cycle );

            UE_LOG(LogTemp, Warning, TEXT("new cycle"));
            ret = true;
        }
            if ( ret == false ) UE_LOG(LogTemp, Warning, TEXT("NOT CHORDLESS"));
            if ( ret == true  ) UE_LOG(LogTemp, Warning, TEXT("CHORDLESS"));

        UnmarkCycle( iVertexArray, iSectionArray );
    }

    return ret;
}

// Gary's Third Idea.
void
UOdysseyVectorGroupPaint::March( UOdysseyVectorVertex* iVertex
                               , uint32 maxVertex
                               , std::list<FOdysseyVectorSection*>& iSectionList )
{
    if( /*maxVertex*/1 )
    {
        FCycleNode* nodeMemArea = ( FCycleNode* ) calloc( /*maxVertex*/500, sizeof( FCycleNode ) );
        FCycleNode* nodeArray = nodeMemArea;
        uint32 vertexCount, vertexFrom;
        bool keepProcessing;
        uint32 depth = 0;

        iVertex->SetVisited( true );
        iVertex->SetNode( &nodeArray[0] );

        nodeArray[0].depth   = depth++;
        nodeArray[0].parent  = nullptr;
        nodeArray[0].vertex  = iVertex;
        nodeArray[0].section = nullptr;
        vertexCount = 1;
        vertexFrom = 0;
        keepProcessing = true;

        while( keepProcessing == true )
        {
            uint32 currentCount = vertexCount;
//UE_LOG(LogTemp, Warning, TEXT("vertexCount %d ------------------ ITERATION ----------------------- "), vertexCount );

            for( uint32 i = vertexFrom; i < currentCount && keepProcessing == true; i++ )
            {
                FCycleNode* currentNode = &nodeArray[i];
                UOdysseyVectorVertex* currentVertex = currentNode->vertex;
                std::list<FOdysseyVectorSection*>& sectionList = currentVertex->GetSectionList();
//UE_LOG(LogTemp, Warning, TEXT("checking %d: at %f %f - sectioncount:%d"), currentVertex, currentVertex->GetCoords().x, currentVertex->GetCoords().y, sectionList.size() );
                keepProcessing = false;
/*
                if( currentVertex->IsVisited() == false )
                {
                    currentVertex->SetVisited( true );
*/
                    for( std::list<FOdysseyVectorSection*>::iterator sit = sectionList.begin(); sit != sectionList.end(); ++sit )
                    {
                        FOdysseyVectorSection *currentEdge = (*sit);
//UE_LOG(LogTemp, Warning, TEXT("checking edge %d (%d <----> %d)"), currentEdge, currentEdge->GetVertex(1), currentEdge->GetVertex(0) );
                        /*if( currentEdge->IsVisited() == false )
                        {*/
                            if( currentEdge->IsBlocked( currentNode->vertex ) == false )
                            {
                                UOdysseyVectorVertex* nextVertex = ( currentEdge->GetVertex(0) == currentNode->vertex ) ? currentEdge->GetVertex(1) : 
                                                                                                                          currentEdge->GetVertex(0);
        //UE_LOG(LogTemp, Warning, TEXT("probing next vertex:%d at x:%f y:%f"), nextVertex, nextVertex->GetCoords().x, nextVertex->GetCoords().y );
                                if( ( currentNode->parent == nullptr ) || ( nextVertex != currentNode->parent->vertex ) )
                                {
        //UE_LOG(LogTemp, Warning, TEXT("non-blocked edge:%d"), currentEdge );
                                        if( nextVertex->IsVisited() == true )
                                        {
                                            if( GetStartSection( nextVertex->GetNode() ) != GetStartSection( currentNode ) )
                                            {
                                                std::vector<UOdysseyVectorVertex*> vertexArray(0);
                                                std::vector<FOdysseyVectorSection*> sectionArray(0);
                                                ::ULIS::FVec2D minCoord;
                //UE_LOG(LogTemp, Warning, TEXT("candidate cycle"));
                                                minCoord = MergeNode( currentNode
                                                                    , currentEdge
                                                                    , nullptr
                                                                    , nextVertex->GetNode()
                                                                    , vertexArray
                                                                    , sectionArray );

                                                if( MakeCycle( minCoord, vertexArray, sectionArray ) )
                                                {
                                                    keepProcessing = false;

                                                    break;
                                                }
                                            }
                                        }
                                        else
                                        {
            //UE_LOG(LogTemp, Warning, TEXT("setting %d as visited"), nextVertex );
                                            currentEdge->SetVisited( true );

                                            nextVertex->SetVisited( true );
                                            nextVertex->SetNode( &nodeArray[vertexCount] );

                                            nodeArray[vertexCount].depth   = depth;
                                            nodeArray[vertexCount].parent  = &nodeArray[i];
                                            nodeArray[vertexCount].vertex  = nextVertex;
                                            nodeArray[vertexCount].section = currentEdge; // the edge that led to here
                                            vertexCount++;

                                            keepProcessing = true;
                                        }
                                  } //else UE_LOG(LogTemp, Warning, TEXT("VISITED edge:%d"), currentEdge );
                            } //else UE_LOG(LogTemp, Warning, TEXT("BLOCKED edge:%d"), currentEdge );

                        /*} else UE_LOG(LogTemp, Warning, TEXT("VISITED edge:%d"), currentEdge );*/
                    }
               /* }*/
            }

            depth++;

            vertexFrom = currentCount;
        }

        // cleaning part
        for( uint32 i = 0; i < vertexCount; i++ )
        {
            nodeArray[i].vertex->SetVisited( false );

            if( nodeArray[i].section )
            {
                /*nodeArray[i].section->SetVisited( false );*/
                /*nodeArray[i].section->SetInCycle( false );*/
            }
        }

        // TODO: free bypass as well
        free( nodeMemArea );
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
    //UE_LOG( LogTemp, Warning, TEXT("Simplify") );
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

    //UE_LOG( LogTemp, Warning, TEXT("Intersections:%d"), iIntersectionVertexList.size() );
}

void
UOdysseyVectorGroupPaint::FindCycles()
{
    std::list<UOdysseyVectorVertexIntersection*> intersectionVertexList;
    std::list<FOdysseyVectorSection*> sectionList;

    // clear mLoopList
    while( mLoopList.size() )
    {
        FOdysseyVectorLoop* cycle = mLoopList.back();

        delete cycle;

        mLoopList.pop_back();
    }

    BuildGraph( intersectionVertexList, sectionList );

    while( intersectionVertexList.size() )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = intersectionVertexList.back();

        UE_LOG(LogTemp,Warning,TEXT("Vertex:%d - x:%f y:%f"),intersectionVertex, intersectionVertex->GetCoords().x, intersectionVertex->GetCoords().y );

        std::vector<UOdysseyVectorVertex*> nodeArray;
        std::vector<FOdysseyVectorSection*> edgeArray;

        March( intersectionVertex, 0, sectionList );

        intersectionVertexList.pop_back();
    }
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::CopyShape()
{
    return NewObject<UOdysseyVectorGroupPaint>();
}
