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
