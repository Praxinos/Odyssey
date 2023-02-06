#include "OdysseyVectorGroupPaint.h"

// MUST be even number
#define EDGESUBSAMPLES 8

UOdysseyVectorGroupPaint::~UOdysseyVectorGroupPaint()
{
    if( mNodeMemoryPool )
    {
        free ( mNodeMemoryPool );
    }

    ClearCycles();
}

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
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        if( cycle->HitTest( iBucket->GetCoords().x, iBucket->GetCoords().y ) )
        {
            cycle->SetBucket( iBucket );

            // a single bucket per loop;
            return;
        }
    }

    /*Invalidate();*/
}

FOdysseyVectorBucket*
UOdysseyVectorGroupPaint::GetBucket( double iX, double iY )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        // TODO: Bounding volume for cycles for faster search
        if( cycle->HitTest( iX, iY ) == true )
        {
            return cycle->GetBucket();
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
        for( int i = 0; i < mLoopArray.size(); i++ )
        {
            FOdysseyVectorLoop *cycle = mLoopArray[i];

            cycle->Draw( iRoi, iFlags );
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
                                          , std::vector<UOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    uint32 intersectionCount = 0;

    for( std::list<UOdysseyVectorSegment*>::iterator sit = cubicSegmenList.begin(); sit != cubicSegmenList.end(); ++sit )
    {
        UOdysseyVectorSegmentCubic *intersectSegment = Cast<UOdysseyVectorSegmentCubic>(*sit);
        ::ULIS::FRectD intersectRect = intersectSegment->GetBoundingBox() & iCubicSegment.GetBoundingBox();

        if( intersectRect.Area() )
        {
            intersectionCount += iCubicSegment.Intersect( *intersectSegment, iIntersectionVertexArray );
        }
    }

    return intersectionCount;
}

static ::ULIS::FVec2D
GetSectionMiddlePoint( FOdysseyVectorSection& iSection )
{
    UOdysseyVectorSegment* segment = iSection.GetSegment();
    double T0 = iSection.GetVertex(0)->GetT(*segment);
    double T1 = iSection.GetVertex(1)->GetT(*segment);
    int stepCount = EDGESUBSAMPLES;
    double deltaT = ( T1 - T0 );
    double stepT = deltaT / stepCount;
    double sampleT = T0 + ( stepT * EDGESUBSAMPLES * 0.5f );

    return segment->GetPointAt( sampleT );
}

static bool
InsideCycleApprox( ::ULIS::FVec2D& iFromCoord
                 , FOdysseyVectorSection& iTestedSection
                 , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    ::ULIS::FVec2D vertexPosition = iTestedSection.GetVertex(0)->IsInCycle() ? iTestedSection.GetVertex(1)->GetCoords() :
                                                                               iTestedSection.GetVertex(0)->GetCoords();
    ::ULIS::FVec2D fromCoord[3] = { { iFromCoord.x - 10.0f, iFromCoord.y - 10.0f }
                                  , { iFromCoord.x - 10.0f, iFromCoord.y         }
                                  , { iFromCoord.x        , iFromCoord.y - 10.0f } };
    uint32 hitCount[3] = { 0, 0, 0 };
    uint32 oddCount = 0;

    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        FOdysseyVectorSection* edge = iEdgeArray[i];

        for( int k = 0; k < 3; k++ )
        {
            if( FOdysseyVector::IntersectSegment( fromCoord[k]
                                                , vertexPosition
                                                , edge->GetVertex(0)->GetCoords()
                                                , edge->GetVertex(1)->GetCoords()
                                                , nullptr
                                                , nullptr ) == true )
            {
                hitCount[k]++;
            }
        }
    }

    if( hitCount[0] % 2 ) oddCount++;
    if( hitCount[1] % 2 ) oddCount++;
    if( hitCount[2] % 2 ) oddCount++;

    return ( oddCount >= 2 ) ? true : false;
}

// Ray casting algorithm. Draw a virtual ray from outside the cycle to the point.
// Count how many times it hit one of the cycle's edge. If that number is odd, it's inside.
// otherwise it's outside.
// https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
static bool
InsideCyclePrecise( ::ULIS::FVec2D& iFromCoord, FOdysseyVectorSection& iTestedSection, std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    /*UOdysseyVectorSegment* testedSegment = iTestedSection.GetSegment();
    double testedVertexT = ( iTestedSection.GetVertex(0)->GetT(*testedSegment)
                           + iTestedSection.GetVertex(1)->GetT(*testedSegment) ) * 0.5f;*/
    ::ULIS::FVec2D vertexPosition = GetSectionMiddlePoint( iTestedSection ) /*testedSegment->GetPointAt( testedVertexT )*/;
    uint32 hitCount[3] = { 0, 0, 0 };
    ::ULIS::FVec2D fromCoord[3] = { { iFromCoord.x - 10.0f, iFromCoord.y - 10.0f }
                                  , { iFromCoord.x - 10.0f, iFromCoord.y         }
                                  , { iFromCoord.x        , iFromCoord.y - 10.0f } };
    uint32 oddCount = 0;

//UE_LOG(LogTemp, Warning, TEXT("testing Vertex position %f %f away from %f %f"), vertexPosition.x, vertexPosition.y, iFromCoord.x, iFromCoord.y  );

    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        FOdysseyVectorSection* edge = iEdgeArray[i];
        UOdysseyVectorSegmentCubic* cubicSegment = Cast<UOdysseyVectorSegmentCubic>(edge->GetSegment());
        double T0 = edge->GetVertex(0)->GetT(*cubicSegment);
        double T1 = edge->GetVertex(1)->GetT(*cubicSegment);
        int stepCount = EDGESUBSAMPLES;
        double deltaT = ( T1 - T0 );
        double stepT = deltaT / stepCount;
        double startT = T0;
        double endT;

        for( int j = 0; j < stepCount; j++ )
        {
            endT = startT + stepT;

            ::ULIS::FVec2D startPoint = ( j == 0 ) ? edge->GetVertex(0)->GetCoords() : cubicSegment->GetPointAt( startT );
            ::ULIS::FVec2D endPoint   = ( j == stepCount - 1 ) ? edge->GetVertex(1)->GetCoords() : cubicSegment->GetPointAt( endT   );
/*
UE_LOG(LogTemp, Warning, TEXT("testing sub-segment x:%f y:%f --- x:%f y:%f"), startPoint.x, startPoint.y, endPoint.x, endPoint.y );
*/
            for( int k = 0; k < 3; k++ )
            {
                if( FOdysseyVector::IntersectSegment( fromCoord[k]
                                                    , vertexPosition
                                                    , startPoint/*edge->GetVertex(0)->GetCoords()*/
                                                    , endPoint/*edge->GetVertex(1)->GetCoords()*/
                                                    , nullptr
                                                    , nullptr ) == true )
                {
                    hitCount[k]++;

    /*UE_LOG(LogTemp, Warning, TEXT("Collides with edge %d"), edge );*/
                }
            }

            startT = endT;
        }
    }

    if( hitCount[0] % 2 ) oddCount++;
    if( hitCount[1] % 2 ) oddCount++;
    if( hitCount[2] % 2 ) oddCount++;

//UE_LOG(LogTemp, Warning, TEXT("hitCount %d"), hitCount );

    return ( oddCount >= 2 ) ? true : false;
}

static bool
InsideCycle( ::ULIS::FVec2D& iFromCoord
           , FOdysseyVectorSection& iTestedSection
           , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    if( ( iTestedSection.GetVertex(0)->IsInCycle() == true )
      &&( iTestedSection.GetVertex(1)->IsInCycle() == true ) )
    {
        return InsideCyclePrecise( iFromCoord, iTestedSection, iEdgeArray );
    }
    else
    {
        return InsideCycleApprox( iFromCoord, iTestedSection, iEdgeArray );
    }

    return false;
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
        iVertexArray[i]->SetInCycle( true, i );
    }
}

static uint64
UnmarkCycle( std::vector<UOdysseyVectorVertex*>& iVertexArray
           , std::vector<FOdysseyVectorSection*>& iEdgeArray )
{
    uint64 cycleID = 0;

    for( int i = 0; i < iEdgeArray.size(); i++ )
    {
        iEdgeArray[i]->SetInCycle( false );

        cycleID = cycleID ^ ( uint64 ) iEdgeArray[i];
    }

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        iVertexArray[i]->SetInCycle( false, 0 );
    }

    return cycleID;
}

static ::ULIS::FVec2D
GetSectionAverage( FOdysseyVectorSection &iSection )
{
    /*
    ::ULIS::FVec2D& point0 = iSection.GetVertex(0)->GetCoords();
    ::ULIS::FVec2D& point1 = iSection.GetVertex(1)->GetCoords();
     ::ULIS::FVec2D average = ( point0 + point1 ) * 0.5f;
    */
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

    
    return average;
}

static void
PrintNode( std::vector<UOdysseyVectorVertex*>& vertexArray
         ,std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), sectionArray[i]->GetVertex(0)->GetCoords().x, sectionArray[i]->GetVertex(0)->GetCoords().y, sectionArray[i]->GetVertex(1), sectionArray[i]->GetVertex(1)->GetCoords().x, sectionArray[i]->GetVertex(1)->GetCoords().y );
    }
}

static uint64
MergeNode( FCycleNode* iNode
         , std::vector<UOdysseyVectorVertex*>& vertexArray
         , std::vector<FOdysseyVectorSection*>& sectionArray )
{
    FCycleNode* node = iNode;
    uint32 nodeCount = node->depth + 1;
    uint64 cycleID = 0;

    if( nodeCount )
    {
        int vrank = 0;
        int srank = 0;

        vertexArray.resize( nodeCount );
        sectionArray.resize( nodeCount );

        while( node )
        {
            ::ULIS::FVec2D& nodeCoord = node->vertex->GetCoords();
            int i = node->depth;
            int p = ( i - 1 + nodeCount ) % nodeCount;

            vertexArray[i]  = node->vertex;
            sectionArray[p] = node->section;

            cycleID = cycleID ^ (uint64) node->section;

            node = node->parent;
        }
    }

    //PrintNode( vertexArray, sectionArray );

    return cycleID;
}

static bool
ReduceCycle( ::ULIS::FVec2D& iFromCoord
           , std::vector<UOdysseyVectorVertex*>& iVertexArray
           , std::vector<FOdysseyVectorSection*>& iSectionArray
           , std::vector<UOdysseyVectorVertex*>& oVertexArray
           , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    UOdysseyVectorVertex* firstVertex = iVertexArray.front();
    UOdysseyVectorVertex* lastVertex = iVertexArray.back();
    FOdysseyVectorSection* firstSection = iSectionArray.front();
    FOdysseyVectorSection* lastSection = iSectionArray.back();
    bool reduced = false;
    int i = 0;
    //static int infiniteLoopDetector = 0;

    oVertexArray.push_back( firstVertex );

    while( oVertexArray.back() != lastVertex )
    {
        int n = ( i + 1 ) % iVertexArray.size();
        UOdysseyVectorVertex* vertex = iVertexArray[i];
        FOdysseyVectorSection* section = iSectionArray[i];
        UOdysseyVectorVertex* nextVertex = iVertexArray[n];
        std::list<FOdysseyVectorSection*>& vertexSectionList = vertex->GetSectionList();
        FOdysseyVectorSection* betterForwardSection = section;
        UOdysseyVectorVertex* betterInsideVertex = nextVertex;

        // increment must be done here, before the skipping process that could override it.
        i++;
 //UE_LOG(LogTemp,Warning,TEXT("PB0"));
        // stop checking after the first detection but keep processing original vertices at the end of the while loop
        if( reduced == false )
        {
 //UE_LOG(LogTemp,Warning,TEXT("PB1"));

//if( infiniteLoopDetector++ > 1000 ) { oSectionArray.clear(); oVertexArray.clear(); return false; }
            // check better point inside
            for( std::list<FOdysseyVectorSection*>::iterator sit = vertexSectionList.begin(); sit != vertexSectionList.end(); ++sit )
            {
                FOdysseyVectorSection *edge = (*sit);

 //UE_LOG(LogTemp,Warning,TEXT("reducing edge:%d x:%f y:%f --- x:%f y:%f"), edge, edge->GetVertex(0)->GetCoords().x, edge->GetVertex(0)->GetCoords().y, edge->GetVertex(1)->GetCoords().x, edge->GetVertex(1)->GetCoords().y );

                if( edge->IsInCycle() == false )
                {
 //UE_LOG(LogTemp,Warning,TEXT("PB4"));
                    if ( InsideCycle( iFromCoord, *edge, iSectionArray ) == true )
                    {
                        FOdysseyVectorSection* insideForwardSection = edge;
                        UOdysseyVectorVertex* insideNextVertex = ( insideForwardSection->GetVertex(0) == vertex ) ? insideForwardSection->GetVertex(1)
                                                                                                                  : insideForwardSection->GetVertex(0);

 //UE_LOG(LogTemp,Warning,TEXT("PB5"));
                        betterForwardSection = insideForwardSection;
                        betterInsideVertex = insideNextVertex;

                        while( insideNextVertex->IsInCycle() == false )
                        {
                            std::list<FOdysseyVectorSection*>& insideVertexSectionList = insideNextVertex->GetSectionList();
//if( infiniteLoopDetector++ > 1000 ) { oSectionArray.clear(); oVertexArray.clear(); return false; }
                            oSectionArray.push_back( insideForwardSection );
                            oVertexArray.push_back( insideNextVertex );

            //UE_LOG(LogTemp,Warning,TEXT("Skipping to x:%f y:%f"), insideNextVertex->GetCoords().x, insideNextVertex->GetCoords().y );

                            for( std::list<FOdysseyVectorSection*>::iterator bsit = insideVertexSectionList.begin(); bsit != insideVertexSectionList.end(); ++bsit)
                            {
                                insideForwardSection = (*bsit);
                                insideNextVertex = ( insideForwardSection->GetVertex(0) == insideNextVertex ) ? insideForwardSection->GetVertex(1)
                                                                                                              : insideForwardSection->GetVertex(0);

                                betterForwardSection = insideForwardSection;
                                betterInsideVertex = insideNextVertex;

                                break; // break for
                            }
                        }

                        i = betterInsideVertex->GetCycleID();

                        reduced = true;

                        break; // break for
                    }
                }
            }
        }

        oSectionArray.push_back( betterForwardSection );
        oVertexArray.push_back( betterInsideVertex );
    }

    oSectionArray.push_back( lastSection );

    return reduced;
}

static uint64
MakeChordless( ::ULIS::FVec2D& iFromCoord
             , std::vector<UOdysseyVectorVertex*>& iVertexArray
             , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    bool reduced;
    std::vector<UOdysseyVectorVertex*> refinedVertexArray;
    std::vector<FOdysseyVectorSection*> refinedSectionArray;
    int loopDetection = 0;
    uint64 cycleID = 0;

    do
    {
        refinedVertexArray.clear();
        refinedSectionArray.clear();

        //UE_LOG(LogTemp,Warning,TEXT("before filtering"));
        //PrintNode(iVertexArray,iSectionArray);

        MarkCycle( iVertexArray, iSectionArray );

        reduced = ReduceCycle( iFromCoord
                             , iVertexArray
                             , iSectionArray
                             , refinedVertexArray
                             , refinedSectionArray );

        cycleID = UnmarkCycle( iVertexArray, iSectionArray );

        iVertexArray = refinedVertexArray;
        iSectionArray = refinedSectionArray;

        //UE_LOG(LogTemp,Warning,TEXT("After filtering"));
        //PrintNode( iVertexArray, iSectionArray );

        /*if( loopDetection++ > 10 ) break;*/
    }
    while ( reduced == true );

    return cycleID;
}

FOdysseyVectorLoop*
UOdysseyVectorGroupPaint::HasCycle( uint64 iID )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        if( mLoopArray[i]->GetID() == iID )
        {
            return mLoopArray[i];
        }
    }

    return false;
}

FOdysseyVectorLoop*
UOdysseyVectorGroupPaint::MakeCycle( uint64 iCycleID
                                   , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                   , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    /*uint64 iCycleID = FOdysseyVectorLoop::GenerateID( iSectionArray );
    FOdysseyVectorLoop* cycle = FOdysseyVectorLoop::Exists( iCycleID, iVertexArray, iSectionArray );*/
    FOdysseyVectorLoop* cycle = nullptr;
    ::ULIS::FVec2D minCoord( mBBox.x, mBBox.y );

//UE_LOG(LogTemp,Warning,TEXT("candidate cycle") );
//PrintNode( iVertexArray, iSectionArray );
// TESTING //

//UE_LOG( LogTemp, Warning, TEXT("Bounding: %f %f %f %f"), mBBox.x, mBBox.y, mBBox.w, mBBox.h );
    if( iVertexArray.size() > 3 )
    {
        iCycleID = MakeChordless( minCoord, iVertexArray, iSectionArray );
    }

    /*iCycleID = FOdysseyVectorLoop::GenerateID( iSectionArray );*/

    /*cycle = HasCycle( iCycleID );*/

    //UE_LOG(LogTemp,Warning,TEXT("cycle ID %X %d"), iCycleID, cycle );

    if( HasCycle( iCycleID ) == nullptr )
    {
        {
            cycle = new FOdysseyVectorLoop( *this, iCycleID, iVertexArray, iSectionArray );

            mLoopArray.push_back( cycle );

//PrintNode( iVertexArray, iSectionArray );
        }

    } //else UE_LOG(LogTemp,Warning,TEXT("cycle exists !!!!!!!!!!!!!!!!!!!!!!!!!!!!") );

    return cycle;
}

static void
CleanNodeArray( FCycleNode* iNodeArray, int size )
{
    for( int i = 0; i < size; i++ )
    {
        iNodeArray[i].vertex->SetVisited( false );
    }
}

// Gary's Third Idea.
FOdysseyVectorLoop*
UOdysseyVectorGroupPaint::March( UOdysseyVectorVertexIntersection* iVertex
                               , FOdysseyVectorSection* iStartSection
                               , FOdysseyVectorSection* iEndSection )
{
    FCycleNode* nodeArray = mNodeMemoryPool;
    uint32 vertexCount, vertexFrom;
    bool keepProcessing;

    nodeArray[0].depth    = 0;
    nodeArray[0].parent   = nullptr;
    nodeArray[0].section  = iEndSection;
    nodeArray[0].vertex   = iVertex;
    nodeArray[0].vertex->SetVisited( true );

    nodeArray[1].depth    = 1;
    nodeArray[1].parent   = &nodeArray[0];
    nodeArray[1].section  = iStartSection;
    nodeArray[1].vertex   = ( iStartSection->GetVertex(0) == iVertex ) ? iStartSection->GetVertex(1)
                                                                       : iStartSection->GetVertex(0);
    nodeArray[1].vertex->SetVisited( true );

    vertexCount = 2;
    vertexFrom  = 1;

    keepProcessing = true;

    while( keepProcessing == true )
    {
        keepProcessing = false;

        uint32 currentCount = vertexCount;
//UE_LOG(LogTemp,Warning,TEXT("vertexFrom:%d currentCount:%d"), vertexFrom, currentCount );
        for( uint32 i = vertexFrom; i < currentCount; i++ )
        {
            FCycleNode* currentNode = &nodeArray[i];
            std::list<FOdysseyVectorSection*>& sectionList = currentNode->vertex->GetSectionList();

            for( std::list<FOdysseyVectorSection*>::iterator sit = sectionList.begin(); sit != sectionList.end(); ++sit )
            {
                FOdysseyVectorSection *currentEdge = (*sit);
                UOdysseyVectorVertex* nextVertex = ( currentEdge->GetVertex(0) == currentNode->vertex ) ? currentEdge->GetVertex(1) : 
                                                                                                          currentEdge->GetVertex(0);

                if( currentEdge != currentNode->section ) // dont go backwards
                {
                    if( /*( currentEdge == iEndSection )
                     || */( currentEdge->IsBlocked( currentNode->vertex ) == false ) )
                    {
//UE_LOG(LogTemp,Warning,TEXT("Exploring edge %d (x:%f y:%f --- x:%f y:%f"), currentEdge, currentNode->vertex->GetCoords().x, currentNode->vertex->GetCoords().y, nextVertex->GetCoords().x, nextVertex->GetCoords().y );
                        if( nextVertex == nodeArray[0].vertex )
                        {
//UE_LOG(LogTemp,Warning,TEXT("loop ?") );
                            if( currentEdge == iEndSection )
                            {
//UE_LOG(LogTemp,Warning,TEXT("candidate cycle") );
                                std::vector<UOdysseyVectorVertex*> vertexArray(0);
                                std::vector<FOdysseyVectorSection*> sectionArray(0);
                                uint64 cycleID = MergeNode( currentNode
                                                          , vertexArray
                                                          , sectionArray );

//UE_LOG(LogTemp,Warning,TEXT("candidate cycle of size:%d"), vertexArray.size() );

//PrintNode( vertexArray, sectionArray );
// TODO : check it does not exists here or move GetNormalVector in MakeCycle

                                // Side note: MakeCycle also increments the Cycle count for each section.
                                FOdysseyVectorLoop* cycle = MakeCycle( cycleID, vertexArray, sectionArray );

                                if( cycle )
                                {
//PrintNode( vertexArray, sectionArray );
//UE_LOG(LogTemp,Warning,TEXT("cycle created: --------------------------- ") );
                                    CleanNodeArray( nodeArray, vertexCount );

                                    // return now, don't let it find another cycle;
                                    return cycle;
                                }
                            }
                        }

                        if( nextVertex->IsVisited() == false )
                        {
                            nextVertex->SetVisited( true );

//UE_LOG(LogTemp, Warning, TEXT("++:%d"), currentNode->ID );
                            nodeArray[vertexCount].depth    = currentNode->depth + 1;
                            nodeArray[vertexCount].parent   = currentNode;
                            nodeArray[vertexCount].vertex   = nextVertex;
                            nodeArray[vertexCount].section  = currentEdge; // the edge that led to here
                            vertexCount++;

                            keepProcessing = true;
                        }
                    }
                }
            }
        }

        vertexFrom = currentCount;
    }

    CleanNodeArray( nodeArray, vertexCount );

    // TODO: test with a return value of number of cycles detected ?
    return nullptr;
}

void
UOdysseyVectorGroupPaint::SimplifyGraph()
{
    std::list<FOdysseyVectorSection*> sectionList;
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

                sectionList.insert( sectionList.end(), segmentSectionList.begin(), segmentSectionList.end() );
            }
        }
    }

    do
    {
        keepSimplifying = false;
    //UE_LOG( LogTemp, Warning, TEXT("Simplify") );
    //UE_LOG( LogTemp, Warning, TEXT("Sections:%d"), sectionList.size() );
        sectionList.remove_if( [&keepSimplifying]( FOdysseyVectorSection *section )
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

uint32
UOdysseyVectorGroupPaint::BuildGraph( std::vector<UOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    std::list<UOdysseyVectorSegment*> cubicSegmenList;
    UOdysseyVectorSegmentCubic *cubicSegment;
    uint32 intersectionCount = 0;
    uint32 vertexCount = 0;

    for( std::list<UOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        UOdysseyVectorObject *child = (*oit);

        if( child->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
        {
            UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(child);
            std::list<UOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();

            mBBox = ( vertexCount == 0 ) ? cubicPath->GetBBox( false ) : mBBox | cubicPath->GetBBox( false );

            vertexCount += cubicPath->GetVertexList().size();

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
        intersectionCount += IntersectSegment ( *cubicSegment, cubicSegmenList, iIntersectionVertexArray );

        // remove segment from list as they are tested
        cubicSegmenList.pop_back();

        cubicSegment = cubicSegmenList.size() ? Cast<UOdysseyVectorSegmentCubic>( cubicSegmenList.back() ) : nullptr;
    }

    // Step Two - simply the graph by removing sections that are not part of a cycle. to do that we proceed with several passes
    // by removing the sections that are at a dead-end and go-on until no section is a dead-end.

    SimplifyGraph();

    //UE_LOG( LogTemp, Warning, TEXT("Intersections:%d"), iIntersectionVertexList.size() );

    return intersectionCount + vertexCount;
}

void
UOdysseyVectorGroupPaint::MarchVertex( UOdysseyVectorVertexIntersection& iVertex )
{
    UOdysseyVectorSegment* primarySegment = iVertex.GetFirstSegment();
    std::list<FOdysseyVectorSection*>& vertexSectionList = iVertex.GetSectionList();

    UE_LOG( LogTemp, Warning, TEXT("Intersection vertices:%d x:%f y:%f"), &iVertex, iVertex.GetCoords().x, iVertex.GetCoords().y );

    for( std::list<FOdysseyVectorSection*>::iterator pit = vertexSectionList.begin(); pit != vertexSectionList.end(); ++pit )
    {
        FOdysseyVectorSection *endSection = (*pit);

        if( endSection->GetSegment() == primarySegment )
        {
            if( endSection->IsBlocked( endSection->GetOtherVertex( &iVertex ) ) == false )
            {
                for( std::list<FOdysseyVectorSection*>::iterator sit = vertexSectionList.begin(); sit != vertexSectionList.end(); ++sit )
                {
                    FOdysseyVectorSection *startSection = (*sit);

                    if( startSection->GetSegment() != primarySegment )
                    {
                        if( startSection->IsBlocked( &iVertex ) == false )
                        {
                            FOdysseyVectorLoop* cycle = March( &iVertex, startSection, endSection );

                            if( cycle )
                            {
                                MarchCycle( *cycle );
                            }
                        }
                    }
                }
            }
        }
    }
}

void
UOdysseyVectorGroupPaint::MarchCycle( FOdysseyVectorLoop& iCycle )
{
    std::vector<UOdysseyVectorVertex*>& vertexArray = iCycle.GetVertexArray();
    std::vector<FOdysseyVectorSection*>& sectionArray = iCycle.GetSectionArray();
    std::vector<FOdysseyVectorLoop*> childCycleArray;
    uint32 cycleValence = vertexArray.size();

    iCycle.SetMarched( true );

    /*iCycle.Block();*/

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        int p = ( i - 1 + cycleValence ) % cycleValence;

        if( vertexArray[i]->GetClass() == UOdysseyVectorVertexIntersection::StaticClass() )
        {
            UOdysseyVectorVertexIntersection* intersectionVertex = Cast<UOdysseyVectorVertexIntersection>(vertexArray[i]);
            FOdysseyVectorSection* startSection = sectionArray[p];

            if( startSection->IsBlocked( intersectionVertex ) == false )
            {
                FOdysseyVectorSection* discardSection = sectionArray[i];

                FOdysseyVectorSection* endSection = intersectionVertex->GetCrossingSection( startSection->GetSegment(), discardSection );
    //UE_LOG(LogTemp,Warning,TEXT("Marching %d ? at vertex:%d x:%f y:%f"), i, intersectionVertex, intersectionVertex->GetCoords().x, intersectionVertex->GetCoords().y );
                if( endSection && ( endSection->IsBlocked( endSection->GetOtherVertex( intersectionVertex ) ) == false ) )
                {
    //UE_LOG(LogTemp,Warning,TEXT("Marching Child %d at vertex:%d x:%f y:%f"), i, intersectionVertex, intersectionVertex->GetCoords().x, intersectionVertex->GetCoords().y );
    //UE_LOG(LogTemp,Warning,TEXT("Allowed start section %d x:%f y:%f ---- x:%f y:%f"),startSection,startSection->GetVertex(0)->GetCoords().x, startSection->GetVertex(0)->GetCoords().y, startSection->GetVertex(1)->GetCoords().x, startSection->GetVertex(1)->GetCoords().y );
    //UE_LOG(LogTemp,Warning,TEXT("Allowed end section %d x:%f y:%f ---- x:%f y:%f"),endSection, endSection->GetVertex(0)->GetCoords().x,endSection->GetVertex(0)->GetCoords().y,endSection->GetVertex(1)->GetCoords().x,endSection->GetVertex(1)->GetCoords().y);

                    FOdysseyVectorLoop* childCycle = March( intersectionVertex, startSection, endSection );

                    if( childCycle )
                    {
                        childCycleArray.push_back( childCycle );
                    }

                    // Block both ways to prevent cycle redetection
                    startSection->BlockAll();
                }
            }
        }
    }

    /*iCycle.UnBlock();*/

    for( int i = 0; i < childCycleArray.size(); i++ )
    {
        if( childCycleArray[i]->IsMarched() == false )
        {
            MarchCycle( *childCycleArray[i] );
        }
    }
}

void
UOdysseyVectorGroupPaint::ClearCycles()
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        delete cycle;
    }

    mLoopArray.clear();
}

void
UOdysseyVectorGroupPaint::FindCycles()
{
    std::vector<UOdysseyVectorVertexIntersection*> intersectionVertexArray;
    uint32 totalVertexCount = 0;

    // clear mLoopList
    ClearCycles();

    intersectionVertexArray.reserve( 500 );

    totalVertexCount = BuildGraph( intersectionVertexArray );

    mNodeMemoryPool = ( FCycleNode* ) realloc ( mNodeMemoryPool, totalVertexCount * sizeof( FCycleNode ) );

    //UE_LOG( LogTemp, Warning, TEXT("Detection -----------------------------------------------------------") );
    //UE_LOG( LogTemp, Warning, TEXT("Intersection vertices:%d"), intersectionVertexList.size() );

    for( int i = 0; i < intersectionVertexArray.size(); i++ )
    {
        MarchVertex( *intersectionVertexArray[i] );
    }

    //UE_LOG( LogTemp, Warning, TEXT("total cycles:%d"), cycleCount );

    Colorize();
}

UOdysseyVectorObject*
UOdysseyVectorGroupPaint::CopyShape()
{
    return NewObject<UOdysseyVectorGroupPaint>();
}

// TODO : bounding box segments.
// TODO: nested cycles.
// TODO: convert path to the group coordinates.
// check number of cycles / valence


UOdysseyVectorHandleBucket*
UOdysseyVectorGroupPaint::PickBucketHandle( double iX, double iY )
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        if( bucket->PickHandle( iX, iY ) )
        {
            return bucket->GetHandle();
        }
    }

    return nullptr;
}
