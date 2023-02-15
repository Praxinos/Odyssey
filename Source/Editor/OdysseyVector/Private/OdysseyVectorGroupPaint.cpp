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

uint32
UOdysseyVectorGroupPaint::FindPath( UOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation )
{
    UOdysseyVectorVertex* nextVertex = ( iSection->GetVertex(0) == iVertex ) ? iSection->GetVertex(1)
                                                                             : iSection->GetVertex(0);
    bool hasPrimary = false;
    bool hasSecondary = false;
    bool hasTertiary = false;
    uint32 ret = UOdysseyVectorGroupPaint::NOCYCLE;

    iVertexArray.push_back( iVertex );
    iSectionArray.push_back( iSection );

    iSection->SetVisited( true );

    /*UE_LOG(LogTemp,Warning,TEXT("Exploring section: x:%f y:%f ---- x:%f y:%f"), iSection->GetVertex(0)->GetCoords().x,
                                                                                iSection->GetVertex(0)->GetCoords().y,
                                                                                iSection->GetVertex(1)->GetCoords().x,
                                                                                iSection->GetVertex(1)->GetCoords().y );*/

    if( nextVertex == iVertexArray.front() ) // cycle detected
    {
/*
        UE_LOG(LogTemp,Warning,TEXT("FOdysseyVectorLoop::Build: Array size %d %d"),iVertexArray.size(),iSectionArray.size());
*/
        iSection->SetVisited( false );

        ret = UOdysseyVectorGroupPaint::HASCYCLE;
    }
    else
    {
        FOdysseyVectorSection* primaryNextSection = nextVertex->GetCycleNextSection( iSection, 1.0f );
        FOdysseyVectorSection* secondaryNextSection = nextVertex->GetOtherSection( iSection );
        FOdysseyVectorSection* tertiaryNextSection = nextVertex->GetCycleNextSection( iSection, -1.0f );

        if( primaryNextSection )
        {
            if( primaryNextSection->IsBlocked( nextVertex ) == false )
            {
                if( ( primaryNextSection->IsVisited() == false ) )
                {
                    ret = FindPath( nextVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation );
                }
                else
                {
                    ret = UOdysseyVectorGroupPaint::NOCYCLE;
                }
            }
            else
            {
                ret = UOdysseyVectorGroupPaint::BLOCKED;
            }
        }

        if( ( ret == UOdysseyVectorGroupPaint::NOCYCLE ) && secondaryNextSection )
        {
            if( secondaryNextSection->IsBlocked( nextVertex ) == false )
            {
                if( ( secondaryNextSection->IsVisited() == false ) )
                {
                    ret = FindPath( nextVertex, secondaryNextSection, iVertexArray, iSectionArray, iOrientation );
                }
                else
                {
                    ret = UOdysseyVectorGroupPaint::NOCYCLE;
                }
            }
            else
            {
                ret = UOdysseyVectorGroupPaint::BLOCKED;
            }
        }

        if( ( ret == UOdysseyVectorGroupPaint::NOCYCLE ) && tertiaryNextSection )
        {
            if( tertiaryNextSection->IsBlocked( nextVertex ) == false )
            {
                if( ( tertiaryNextSection->IsVisited() == false ) )
                {
                    ret = FindPath( nextVertex, tertiaryNextSection, iVertexArray, iSectionArray, iOrientation );
                }
                else
                {
                    ret = UOdysseyVectorGroupPaint::NOCYCLE;
                }
            }
            else
            {
                ret = UOdysseyVectorGroupPaint::BLOCKED;
            }
        }
    }

    iSection->Block( iVertex );
    iSection->SetVisited( false );

    if( ret != UOdysseyVectorGroupPaint::HASCYCLE )
    {
        iVertexArray.pop_back();
        iSectionArray.pop_back();
    }

    return ret;
}

static void
BlockPath( std::vector<UOdysseyVectorVertex*>& iVertexArray
          , std::vector<FOdysseyVectorSection*>& iSectionArray
          , bool iIsContour )
{
    for( int i = 0; i < iSectionArray.size(); i++ )
    {

        iSectionArray[i]->Block( iVertexArray[i] );
    }
}

static double
GetNormalVector( std::vector<UOdysseyVectorVertex*>& iVertexArray
               , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    double z = 0;
    int32 arraySize = iVertexArray.size();

    if( iVertexArray.size() == 1 )
    {
        // unimplemented
    }

    if( iVertexArray.size() == 2 )
    {
        ::ULIS::FVec2D startVector =   iSectionArray[0]->GetVectorFromVertex( iVertexArray[0] );
        ::ULIS::FVec2D endVector   = - iSectionArray[1]->GetVectorFromVertex( iVertexArray[0] );

        z = FOdysseyVector::Cross2D( startVector, endVector );
    }

    if( iVertexArray.size() >= 3 )
    {
        for( int i = 0; i < arraySize; i++ )
        {
            int p = ( i - 1 + arraySize ) % arraySize;
            int n = ( i + 1             ) % arraySize;

            ::ULIS::FVec2D& vpCoords = iVertexArray[p]->GetCoords();
            ::ULIS::FVec2D& viCoords = iVertexArray[i]->GetCoords();
            ::ULIS::FVec2D& vnCoords = iVertexArray[n]->GetCoords();
            ::ULIS::FVec2D vpvi = viCoords - vpCoords;
            ::ULIS::FVec2D vivn = vnCoords - viCoords;

            /*z += ( ( vpvi.y * vivn.x ) - ( vpvi.x * vivn.y ) );*/
    // https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
    // Newell's method
            z += ( ( vnCoords.x - viCoords.x ) * ( vnCoords.y + viCoords.y ) );
        }
    }

    //UE_LOG(LogTemp,Warning,TEXT("Normal Z:%f"), z);

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

        if( section->IsBlocked(iIntersectionVertex) == false ) {
            uint32 ret = FindPath( iIntersectionVertex, section, vertexArray, sectionArray, 1.0f );
/*
            BlockPath( vertexArray, sectionArray, false );
*/
            // there are situations were the algorithm takes the outer ring, in that case it would find a cycle that must be discarded.

            if( ret == UOdysseyVectorGroupPaint::HASCYCLE )
            {
                //UE_LOG(LogTemp,Warning,TEXT("candidate cycle of size:%d (sections :%d)"),vertexArray.size(),sectionArray.size());

                if( /*CheckPath( vertexArray, sectionArray ) == true*/ GetNormalVector( vertexArray, sectionArray ) > 0.0f )
                {
                    //PrintCycle( vertexArray, sectionArray );

                    mLoopArray.push_back( new FOdysseyVectorLoop( *this, /*iCycleID*/0, vertexArray, sectionArray ) );
                }
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

    intersectionVertexArray.reserve( 60 );

    totalVertexCount = BuildGraph( intersectionVertexArray );
/*
    mNodeMemoryPool = ( FCycleNode* ) realloc ( mNodeMemoryPool, totalVertexCount * sizeof( FCycleNode ) );
*/
    //UE_LOG( LogTemp, Warning, TEXT("Detection -----------------------------------------------------------") );
    //UE_LOG( LogTemp, Warning, TEXT("Intersection vertices:%d"), intersectionVertexList.size() );

    for( int i = 0; i < intersectionVertexArray.size(); i++ )
    {
        MarchVertex( intersectionVertexArray[i] );
    }

    //UE_LOG( LogTemp, Warning, TEXT("total cycles:%d"), cycleCount );

    OrderCycles();

    Colorize();
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

    //UE_LOG( LogTemp, Warning, TEXT("Intersections:%d"), iIntersectionVertexList.size() );

    return intersectionCount + vertexCount;
}

void
UOdysseyVectorGroupPaint::OrderCycles()
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop* cycle = mLoopArray[i];

        for( int j = 0; j < mLoopArray.size(); j++ )
        {
           FOdysseyVectorLoop* innerCycle = mLoopArray[j];

           if( cycle != innerCycle )
           {
               if( innerCycle->FitsIn( cycle ) )
               {
                   FOdysseyVectorLoop* parentCycle = innerCycle->GetParentCycle();

                   if( parentCycle )
                   {
                       if( parentCycle->FitsIn( cycle ) == false )
                       {
                           parentCycle->RemoveChild( innerCycle );
                           cycle->AppendChild( innerCycle );
                       }
                   }
                   else
                   {
                       cycle->AppendChild( innerCycle );
                   }
               }
           }
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
