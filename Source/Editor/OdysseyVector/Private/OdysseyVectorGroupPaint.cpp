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
UOdysseyVectorGroupPaint::DrawBuckets( ::ULIS::FRectD& iRoi,uint64 iFlags )
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        bucket->Draw( iRoi, iFlags );
    }
}

void
UOdysseyVectorGroupPaint::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        cycle->Draw( iRoi, iFlags );
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

#ifdef UNUSED
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
        iVertexArray[i]->SetTarget( false );
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

static bool
IsChordless( ::ULIS::FVec2D& iFromCoord
           , std::vector<UOdysseyVectorVertex*>& iVertexArray
           , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    UOdysseyVectorVertex* anyVertex = iVertexArray.front();

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        UOdysseyVectorVertex* vertex = iVertexArray[i];
        std::list<FOdysseyVectorSection*>& vertexSectionList = vertex->GetSectionList();

        for( std::list<FOdysseyVectorSection*>::iterator sit = vertexSectionList.begin(); sit != vertexSectionList.end(); ++sit )
        {
            FOdysseyVectorSection *edge = (*sit);

            if( edge->IsInCycle() == false )
            {
                if ( InsideCycle( iFromCoord, *edge, iSectionArray ) == true )
                {
                    FOdysseyVectorSection* insideForwardSection = edge;
                    UOdysseyVectorVertex* insideNextVertex = ( insideForwardSection->GetVertex(0) == vertex ) ? insideForwardSection->GetVertex(1)
                                                                                                                : insideForwardSection->GetVertex(0);

                    while( insideNextVertex->IsInCycle() == false )
                    {
                        std::list<FOdysseyVectorSection*>& insideVertexSectionList = betterInsideVertex->GetSectionList();

                    }
                }
            }
        }
    }

    return true;
}
#endif

static void
PrintCycle( std::vector<UOdysseyVectorVertex*>& vertexArray
         ,std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), sectionArray[i]->GetVertex(0)->GetCoords().x, sectionArray[i]->GetVertex(0)->GetCoords().y, sectionArray[i]->GetVertex(1), sectionArray[i]->GetVertex(1)->GetCoords().x, sectionArray[i]->GetVertex(1)->GetCoords().y );
    }
}

bool
UOdysseyVectorGroupPaint::FindPath( UOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation )
{
    UOdysseyVectorVertex* nextVertex = ( iSection->GetVertex(0) == iVertex ) ? iSection->GetVertex(1)
                                                                             : iSection->GetVertex(0);
    bool ret = false;

    iVertexArray.push_back( iVertex );
    iSectionArray.push_back( iSection );

    iSection->SetVisited( true );

    if( nextVertex == iVertexArray.front() ) // cycle detected
    {
/*
        UE_LOG(LogTemp,Warning,TEXT("FOdysseyVectorLoop::Build: Array size %d %d"),iVertexArray.size(),iSectionArray.size());
*/
        ret = true;
    }
    else
    {
        FOdysseyVectorSection* nextSection = nextVertex->GetCycleNextSection( iSection, iOrientation );

        if( nextSection )
        {
            if( nextSection->IsBlocked( nextVertex ) == false )
            {
                if( 1/*( nextSection->IsVisited() == false )*/ /*&& ( nextSection->IsBlocked( iVertex ) == false )*/ )
                {
                    if( FindPath( nextVertex, nextSection, iVertexArray, iSectionArray, iOrientation ) )
                    {
                        ret = true;
                    }
                }
            } //else  UE_LOG(LogTemp,Warning,TEXT("blocked"));
        } //else  UE_LOG(LogTemp,Warning,TEXT("no section"));
    }

    iSection->SetVisited( false );

    return ret;
}

static double
GetNormalVector( std::vector<UOdysseyVectorVertex*>& iVertexArray )
{
    double z = 0;
    int32 arraySize = iVertexArray.size();

    for( int i = 0; i < arraySize; i++ )
    {
        int p = ( i - 1 + arraySize ) % arraySize;
        int n = ( i + 1             ) % arraySize;

        if( iVertexArray[i]->GetClass() == UOdysseyVectorVertexIntersection::StaticClass() )
        {
            ::ULIS::FVec2D& vpCoords = iVertexArray[p]->GetCoords();
            ::ULIS::FVec2D& viCoords = iVertexArray[i]->GetCoords();
            ::ULIS::FVec2D& vnCoords = iVertexArray[n]->GetCoords();
            ::ULIS::FVec2D vpvi = viCoords - vpCoords;
            ::ULIS::FVec2D vivn = vnCoords - viCoords;

            z += ( ( vpvi.y * vivn.x ) - ( vpvi.x * vivn.y ) );
            /*z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );*/
        }
    }

    return z;
}

void
UOdysseyVectorGroupPaint::MarchVertex( UOdysseyVectorVertexIntersection* iIntersectionVertex )
{
    std::list<FOdysseyVectorSection*>& sectionList = iIntersectionVertex->GetSectionList();

    //UE_LOG(LogTemp,Warning,TEXT("vertex :%f %f (sections :%d)"), iIntersectionVertex->GetCoords().x, iIntersectionVertex->GetCoords().y, iIntersectionVertex->GetSectionCount() );

    for( std::list<FOdysseyVectorSection*>::iterator pit = sectionList.begin(); pit != sectionList.end(); ++pit )
    {
        FOdysseyVectorSection *section = (*pit);
        std::vector<UOdysseyVectorVertex*> vertexArray;
        std::vector<FOdysseyVectorSection*> sectionArray;

        bool hasCycle = FindPath( iIntersectionVertex, section, vertexArray, sectionArray, 1.0f );

        // there are situations were the algorithm takes the outer ring, in that case it would find a cycle that must be discarded.

        if( hasCycle )
        {
            if( /*CheckPath( vertexArray, sectionArray ) == true*/ GetNormalVector( vertexArray ) > 0.0f )
            {
                //PrintCycle( vertexArray, sectionArray );

                mLoopArray.push_back( new FOdysseyVectorLoop( *this, /*iCycleID*/0, vertexArray, sectionArray ) );
            }
        }
    }
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
        MarchVertex( intersectionVertexArray[i] );
    }

    //UE_LOG( LogTemp, Warning, TEXT("total cycles:%d"), cycleCount );

    Colorize();
}

#ifdef UNUSED
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
        FindPath( *intersectionVertexArray[i],  );
    }

    //UE_LOG( LogTemp, Warning, TEXT("total cycles:%d"), cycleCount );

    Colorize();
}
#endif

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
UOdysseyVectorGroupPaint::ClearCycles()
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        delete cycle;
    }

    mLoopArray.clear();
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
