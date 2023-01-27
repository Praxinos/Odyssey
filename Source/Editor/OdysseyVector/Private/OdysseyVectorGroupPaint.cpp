#include "OdysseyVectorGroupPaint.h"

void
UOdysseyVectorGroupPaint::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorGroupPaint::Colorize()
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        ApplyBucket( bucket );
    }
}

void
UOdysseyVectorGroupPaint::ApplyBucket( FOdysseyVectorBucket* iBucket )
{
    for( std::list<FOdysseyVectorLoop*>::iterator lit = mLoopList.begin(); lit != mLoopList.end(); ++lit )
    {
        FOdysseyVectorLoop *loop = static_cast<FOdysseyVectorLoop*>(*lit);

        if( loop->HitTest( iBucket->GetCoords().x, iBucket->GetCoords().y ) )
        {
            loop->SetBucket( iBucket );

            // a single bucket per loop;
            return;
        }
    }

    /*Invalidate();*/
}

FOdysseyVectorBucket*
UOdysseyVectorGroupPaint::GetBucket( double iX, double iY )
{
    for( std::list<FOdysseyVectorLoop*>::iterator lit = mLoopList.begin(); lit != mLoopList.end(); ++lit )
    {
        FOdysseyVectorLoop *loop = static_cast<FOdysseyVectorLoop*>(*lit);

        // TODO: Bounding volume for cycles for faster search
        if( loop->HitTest( iX, iY ) == true )
        {
            return loop->GetBucket();
        }
    }

    return nullptr;
}

FOdysseyVectorBucket*
UOdysseyVectorGroupPaint::Bucket( uint32 iColor, double iX, double iY )
{
    FOdysseyVectorBucket* bucket = GetBucket( iX, iY );

    if( bucket == nullptr )
    {
        bucket = new FOdysseyVectorBucket( *this, iColor, iX, iY );

        mBucketList.push_back( bucket );
    }

    bucket->SetColor( iColor );

    ApplyBucket( bucket );

    return bucket;
}

void
UOdysseyVectorGroupPaint::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    if( mChildrenList.size() )
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
InsideCycle( ::ULIS::FVec2D& iFromCoord, ::ULIS::FVec2D& testCoord, std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    uint32 hitCount = 0;

    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        FOdysseyVectorSection *edge = iEdgeArray[i];

        if( FOdysseyVector::IntersectSegment( iFromCoord
                                            , testCoord
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

static ::ULIS::FVec2D
GetSectionAverage( FOdysseyVectorSection &iSection )
{
    ::ULIS::FVec2D& point0 = iSection.GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSection.GetVertex(1)->GetCoords();
     ::ULIS::FVec2D average = ( point0 + point1 ) * 0.5f;
    /*
    UOdysseyVectorSegmentCubic& segment = *Cast<UOdysseyVectorSegmentCubic>(iSection.GetSegment());
    ::ULIS::FVec2D& point0 = segment.GetPoint(0)->GetCoords();
    ::ULIS::FVec2D& point1 = segment.GetPoint(1)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint0 = segment.GetControlPoint(0)->GetCoords();
    ::ULIS::FVec2D& ctrlPoint1 = segment.GetControlPoint(1)->GetCoords();
    double t0 = iSection.GetVertex(0)->GetT( segment );
    double t1 = iSection.GetVertex(1)->GetT( segment );
    ::ULIS::FVec2D average = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( point0
                                                                                , ctrlPoint0
                                                                                , ctrlPoint1
                                                                                , point1
                                                                                , ( t0 + t1 ) * 0.5f );
*/
    
    return average;
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
                ::ULIS::FVec2D sectionAverageCoord = GetSectionAverage( *edge );

                if( InsideCycle( iFromCoord, sectionAverageCoord, iEdgeArray ) == true )
                {
//UE_LOG(LogTemp, Warning, TEXT("InsideCycle") );
                    return false;
                }
            } //UE_LOG(LogTemp, Warning, TEXT("edge->IsInCycle()") );
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
        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), sectionArray[i]->GetVertex(0)->GetCoords().x, sectionArray[i]->GetVertex(0)->GetCoords().y, sectionArray[i]->GetVertex(1), sectionArray[i]->GetVertex(1)->GetCoords().x, sectionArray[i]->GetVertex(1)->GetCoords().y );
    }
}

static ::ULIS::FVec2D
MergeNode( FCycleNode* iNodeArray
         , uint32 iTargetID
         , FOdysseyVectorSection* iEdge
         , std::list<FOdysseyVectorSection*>* iBypassEdgeList
         , std::vector<UOdysseyVectorVertex*>& vertexArray
         , std::vector<FOdysseyVectorSection*>& sectionArray )
{
    FCycleNode* node = &iNodeArray[iTargetID];
    uint32 nodeCount = node->depth + 1;
    ::ULIS::FVec2D minCoord = ::ULIS::FVec2D(0,0);

    if( nodeCount )
    {
        int vrank = 0;
        int srank = 0;

        minCoord = node->vertex->GetCoords();

        vertexArray.resize( nodeCount );
        sectionArray.resize( nodeCount );

        vertexArray[0] = iNodeArray[0].vertex;

        // concatenate iNode1 and iNode0 except for the root node.
        while( node->parentID != -1 )
        {
            ::ULIS::FVec2D& nodeCoord = node->vertex->GetCoords();
            int i = node->depth;
            int p = ( i - 1 + nodeCount ) % nodeCount;

            vertexArray[i]  = node->vertex;
            sectionArray[p] = node->section;

            // find minimum point, we'll need it for the ray casting algorithm in the InsideCycle() function.
            // Note: we need it to be outside the cycle, so we arbitrarily substract 0.1f
            if( nodeCoord.x < minCoord.x ) minCoord.x = nodeCoord.x - 0.1f;
            if( nodeCoord.y < minCoord.y ) minCoord.y = nodeCoord.y - 0.1f;

            node = &iNodeArray[node->parentID];
        }

        sectionArray[nodeCount-1] = iEdge;
    }

    //PrintNode( vertexArray, sectionArray );

    return minCoord;
}

// 2 Nodes met at collision point.
// returns 0 = no cycle
// returns 1 = cycle already exists
// returns 2 = new cycle created
uint32
UOdysseyVectorGroupPaint::MakeCycle( ::ULIS::FVec2D& minCoord
                                   , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                   , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    uint64 cycleID = FOdysseyVectorLoop::GenerateID( iSectionArray );
    uint32 ret = 0;

//UE_LOG(LogTemp, Warning, TEXT("CycleID potentiel : %X"), cycleID );

    if( FOdysseyVectorLoop::Exists( cycleID, iVertexArray, iSectionArray ) == false )
    {

        MarkCycle( iVertexArray, iSectionArray );
            //UE_LOG(LogTemp, Warning, TEXT("TEST CHORDLESS"));

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
//PrintNode( iVertexArray, iSectionArray );
            mLoopList.push_back( cycle );

            //UE_LOG(LogTemp, Warning, TEXT("new cycle"));
            ret = 2;
        }
            //if ( ret == 0 ) UE_LOG(LogTemp, Warning, TEXT("NOT CHORDLESS"));
            //if ( ret      ) UE_LOG(LogTemp, Warning, TEXT("CHORDLESS"));

        UnmarkCycle( iVertexArray, iSectionArray );
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static FOdysseyVectorSection*
GetStartSection( FCycleNode* iNodeArray, int32 iEndNodeID )
{
    FCycleNode* node = &iNodeArray[iEndNodeID];
    FOdysseyVectorSection* section = nullptr;

    while( node->parentID != -1 )
    {
        section = node->section;

        node = &iNodeArray[node->parentID];
    }

    return section;
}

// Gary's Third Idea.
void
UOdysseyVectorGroupPaint::March( UOdysseyVectorVertexIntersection* iVertex
                               , std::list<FOdysseyVectorSection*>& iSectionList )
{
    FCycleNode* nodeMemArea = ( FCycleNode* ) malloc( /*maxVertex*/5000 * sizeof( FCycleNode ) );
    FCycleNode* nodeArray = nodeMemArea;
    uint32 vertexCount = 0, vertexFrom;
    bool keepProcessing;
    uint32 depth = 0;
    int32 cycleCount = 0;
    int32 maxCycleCount = ( iVertex->GetSectionCount() - 1 ) * 2;

    /*iVertex->SetVisited( true );
    iVertex->SetNode( &nodeArray[0] );*/

    nodeArray[0].depth    = depth++;
    nodeArray[0].parentID = -1;
    nodeArray[0].ID       = vertexCount;
    nodeArray[0].vertex   = iVertex;
    nodeArray[0].section  = nullptr;

    vertexCount = 1;
    vertexFrom  = 0;

    keepProcessing = true;

    while( keepProcessing == true )
    {
        uint32 currentCount = vertexCount;
//UE_LOG(LogTemp, Warning, TEXT("vertexCount %d ------------------ ITERATION ----------------------- "), vertexCount );

        for( uint32 i = vertexFrom; i < currentCount && ( keepProcessing == true ); i++ )
        {
            FCycleNode* currentNode = &nodeArray[i];
            FCycleNode* parentNode = ( currentNode->parentID != -1 ) ? &nodeArray[currentNode->parentID] : nullptr;
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

                    /*if( currentEdge->IsVisited() == false )
                    {*/
                        /*if( currentEdge->IsBlocked( currentNode->vertex ) == false )
                        {*/
                            UOdysseyVectorVertex* nextVertex = ( currentEdge->GetVertex(0) == currentNode->vertex ) ? currentEdge->GetVertex(1) : 
                                                                                                                        currentEdge->GetVertex(0);

//UE_LOG(LogTemp,Warning,TEXT("Travelling from %f %f --- to ---> %f %f)"),currentNode->vertex->GetCoords().x,currentNode->vertex->GetCoords().y,nextVertex->GetCoords().x,nextVertex->GetCoords().y);

                            if( /*( currentNode->parentID == -1 ) ||*/ ( currentEdge != currentNode->section ) )
                            {
    //UE_LOG(LogTemp, Warning, TEXT("probing next vertex:%d at x:%f y:%f"), nextVertex, nextVertex->GetCoords().x, nextVertex->GetCoords().y );

    //UE_LOG(LogTemp, Warning, TEXT("non-blocked edge:%d"), currentEdge );
                                    if( nextVertex == nodeArray[0].vertex )
                                    {
            //UE_LOG(LogTemp, Warning, TEXT("cycle detected :%d"), currentNode->depth + 1 );
                                        if(   ( depth == 1 ) // segment loops on itself
                                         || ( ( depth >  1 ) && ( GetStartSection( nodeArray, currentNode->ID ) != currentEdge ) ) ) // exit and entry sections are different
                                        {
                                            std::vector<UOdysseyVectorVertex*> vertexArray(0);
                                            std::vector<FOdysseyVectorSection*> sectionArray(0);
                                            ::ULIS::FVec2D minCoord;
            //UE_LOG(LogTemp, Warning, TEXT("candidate cycle of size:%d"), currentNode->depth + 1 );
                                            minCoord = MergeNode( nodeArray
                                                                , currentNode->ID
                                                                , currentEdge
                                                                , nullptr
                                                                , vertexArray
                                                                , sectionArray );

                                            if( MakeCycle( minCoord, vertexArray, sectionArray ) )
                                            {
                                                cycleCount++;
            //UE_LOG(LogTemp, Warning, TEXT("cycleCount:%d/%d"), cycleCount, maxCycleCount );
                                                if( cycleCount == maxCycleCount )
                                                {
                                                    keepProcessing = false;

                                                    /*marchNext = GetFirstIntersectionVertex( vertexArray );*/

                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
        //UE_LOG(LogTemp, Warning, TEXT("setting %d as visited"), nextVertex );
                                        /*currentEdge->SetVisited( true );*/

                                        /*nextVertex->SetVisited( true );*/
                                        /*nextVertex->SetNode( &nodeArray[vertexCount] );*/

                                        nodeArray[vertexCount].depth    = currentNode->depth + 1;
                                        nodeArray[vertexCount].parentID = currentNode->ID;
                                        nodeArray[vertexCount].ID       = vertexCount;
                                        nodeArray[vertexCount].vertex   = nextVertex;
                                        nodeArray[vertexCount].section  = currentEdge; // the edge that led to here
                                        vertexCount++;

                                        keepProcessing = true;
                                    }
                                } //else UE_LOG(LogTemp, Warning, TEXT("VISITED edge:%d"), currentEdge );
                        /*}*/ //else UE_LOG(LogTemp, Warning, TEXT("BLOCKED edge:%d"), currentEdge );

                    /*} else UE_LOG(LogTemp, Warning, TEXT("VISITED edge:%d"), currentEdge );*/
                }
            /* }*/
        }

        /*depth++;*/

        vertexFrom = currentCount;
    }

    // cleaning part
/*
    for( uint32 i = 0; i < vertexCount; i++ )
    {
        nodeArray[i].vertex->SetVisited( false );

        if( nodeArray[i].section )
        {
            //nodeArray[i].section->SetVisited( false );
            //nodeArray[i].section->SetInCycle( false );
        }
    }
*/
    // TODO: free bypass as well
    free( nodeMemArea );
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

    //UE_LOG( LogTemp, Warning, TEXT("Detection -----------------------------------------------------------") );
    //UE_LOG( LogTemp, Warning, TEXT("Intersection vertices:%d"), intersectionVertexList.size() );

    while( intersectionVertexList.size() )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = intersectionVertexList.back();

//UE_LOG(LogTemp,Warning,TEXT("Vertex:%d - x:%f y:%f - valence:%d"),intersectionVertex, intersectionVertex->GetCoords().x, intersectionVertex->GetCoords().y, intersectionVertex->GetSectionCount() );

        std::vector<UOdysseyVectorVertex*> nodeArray;
        std::vector<FOdysseyVectorSection*> edgeArray;

        March( intersectionVertex, sectionList );

        intersectionVertexList.pop_back();
    }

    Colorize();
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::CopyShape()
{
    return NewObject<UOdysseyVectorGroupPaint>();
}
