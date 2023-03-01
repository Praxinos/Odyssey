#include "OdysseyVectorGroupPaint.h"

// MUST be even number
#define EDGESUBSAMPLES 8

UOdysseyVectorGroupPaint::~UOdysseyVectorGroupPaint()
{
    ClearCycles();
}

UOdysseyVectorGroupPaint::UOdysseyVectorGroupPaint()
    : Tolerance( 0.0f )
{
    mDependsOnChildren = true;
}

uint32
UOdysseyVectorGroupPaint::GetType()
{
    return UOdysseyVectorObject::VECTORGROUPPAINTTYPE;
}

void
UOdysseyVectorGroupPaint::Init( std::string iName )
{
    SetName( iName );
}

std::list<FOdysseyVectorBucket*>&
UOdysseyVectorGroupPaint::GetBucketList()
{
    return mBucketList;
}

void
UOdysseyVectorGroupPaint::Colorize()
{
    // reset color for all cycles first
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        cycle->SetBucket( nullptr );
    }

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
UOdysseyVectorGroupPaint::PickBucket( double iX, double iY )
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        if ( bucket->Pick( iX, iY ) )
        {
            return bucket;
        }
    }

    return nullptr;
}

FOdysseyVectorLoop*
UOdysseyVectorGroupPaint::PickCycle( double iX, double iY )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorLoop *cycle = mLoopArray[i];

        // TODO: Bounding volume for cycles for faster search
        if( cycle->HitTest( iX, iY ) == true )
        {
            return cycle;
        }
    }

    return nullptr;
}

FOdysseyVectorBucket*
UOdysseyVectorGroupPaint::Bucket( double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    FOdysseyVectorBucket* bucket = PickBucket( iX, iY );

    if( bucket == nullptr )
    {
        bucket = new FOdysseyVectorBucket( *this, iX, iY );

        AddBucket( bucket );
    }

    bucket->SetColor( iR, iG, iB, iA );

    ApplyBucket( bucket );

    return bucket;
}

void
UOdysseyVectorGroupPaint::UpdateShape( uint32 iUpdateFlags )
{
    UOdysseyVectorGroup::UpdateShape( iUpdateFlags ); // updates BBox

    if(   ( Realtime == true  )
     || ( ( Realtime == false ) && ( ( iUpdateFlags & UOdysseyVectorObject::FREQUENTUPDATES ) == 0 ) ) )
    {
        FindCycles();
    }
}

void
UOdysseyVectorGroupPaint::AddBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.push_back( iBucket );
}

void
UOdysseyVectorGroupPaint::RemoveBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.remove( iBucket );
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

bool
UOdysseyVectorGroupPaint::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        for( int i = 0; i < mLoopArray.size(); i++ )
        {
            FOdysseyVectorLoop *cycle = mLoopArray[i];

            if( cycle->HitTest( pt.x, pt.y ) )
            {
                return true;
            }
        }
    }

    return false;
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
            intersectionCount += iCubicSegment.Intersect( *intersectSegment, Tolerance, iIntersectionVertexArray );
        }
    }

    return intersectionCount;
}

static void
PrintCycle( std::vector<UOdysseyVectorVertex*>& vertexArray
          , std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        UOdysseyVectorSegment* segment = sectionArray[i]->GetSegment();
        BLPoint pt0 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(0)->GetCoordsOnSegment(segment).x, sectionArray[i]->GetVertex(0)->GetCoordsOnSegment(segment).y );
        BLPoint pt1 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(1)->GetCoordsOnSegment(segment).x, sectionArray[i]->GetVertex(1)->GetCoordsOnSegment(segment).y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

uint32
UOdysseyVectorGroupPaint::FindPath( UOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<UOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation
                                  , uint32 iDepth ) // we could also use iVertexArray.size()
{
    UOdysseyVectorVertex* nextVertex = ( iSection->GetVertex(0) == iVertex ) ? iSection->GetVertex(1)
                                                                             : iSection->GetVertex(0);
    bool hasPrimary = false;
    bool hasSecondary = false;
    bool hasTertiary = false;
    uint32 ret = UOdysseyVectorGroupPaint::NOCYCLE;

    iVertexArray.push_back( iVertex );
    iSectionArray.push_back( iSection );

    iVertex->SetVisited( true );
    iSection->Block( iVertex );

    /*UE_LOG(LogTemp,Warning,TEXT("Exploring section: x:%f y:%f ---- x:%f y:%f"), iSection->GetVertex(0)->GetCoords().x,
                                                                                iSection->GetVertex(0)->GetCoords().y,
                                                                                iSection->GetVertex(1)->GetCoords().x,
                                                                                iSection->GetVertex(1)->GetCoords().y );*/

    if( /*nextVertex == iVertexArray.front()*/nextVertex->IsVisited() == true ) // cycle detected
    {
        if( nextVertex == iVertexArray.front() )
        {
            ret = UOdysseyVectorGroupPaint::HASCYCLE;
        }
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
                ret = FindPath( nextVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
            }
            else
            {
                ret = UOdysseyVectorGroupPaint::NOCYCLE;
            }
        }

        if( ( ret == UOdysseyVectorGroupPaint::NOCYCLE ) && secondaryNextSection )
        {
            if( secondaryNextSection->IsBlocked( nextVertex ) == false )
            {
                ret = FindPath( nextVertex, secondaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
            }
            else
            {
                ret = UOdysseyVectorGroupPaint::NOCYCLE;
            }
        }

        if( ( ret == UOdysseyVectorGroupPaint::NOCYCLE ) && tertiaryNextSection )
        {
            if( tertiaryNextSection->IsBlocked( nextVertex ) == false )
            {
                ret = FindPath( nextVertex, tertiaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
            }
            else
            {
                ret = UOdysseyVectorGroupPaint::NOCYCLE;
            }
        }
    }
/*
    iSection->Block( iVertex );
*/
    iSection->UnBlock( iVertex );
    iVertex->SetVisited( false );

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
        // unimplemented (for now)
    }

    if( iVertexArray.size() == 2 )
    {
        ::ULIS::FVec2D startVector =  -iSectionArray[0]->GetVectorFromVertex( iVertexArray[1] );
        ::ULIS::FVec2D endVector   =   iSectionArray[1]->GetVectorFromVertex( iVertexArray[1] );

        z = FOdysseyVector::Cross2D( startVector, endVector );
    }

    if( iVertexArray.size() >= 3 )
    {
        for( int i = 0; i < arraySize; i++ )
        {
            int n = ( i + 1 ) % arraySize;
            UOdysseyVectorSegment* segment = iSectionArray[i]->GetSegment();
            ::ULIS::FVec2D& viCoords = iVertexArray[i]->GetCoordsOnSegment( segment );
            ::ULIS::FVec2D& vnCoords = iVertexArray[n]->GetCoordsOnSegment( segment );
            double ti = iVertexArray[i]->GetT( *segment );
            double tn = iVertexArray[n]->GetT( *segment );
            double deltaT = tn - ti;
            int subdiv = 8;
            double stepT = deltaT / subdiv;
            double t0 = ti;

            // By relying only on start and end points of a section, we lack precision. 
            // Here we rely on more acurate computation by getting intermediate points.
            for( int j = 0; j < subdiv; j++ )
            {
                double t1 = t0 + stepT;
                ::ULIS::FVec2D v0Coords = segment->GetPointAt( t0 );
                ::ULIS::FVec2D v1Coords = segment->GetPointAt( t1 );

                z += ( ( v0Coords.x - v1Coords.x ) * ( v0Coords.y + v1Coords.y ) );

                t0 += stepT;
            }

    // https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
    // Newell's method
            //z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
        }
    }

    //UE_LOG(LogTemp,Warning,TEXT("Normal Z:%f"), z);

    return z;
}

uint32
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
            uint32 ret = FindPath( iIntersectionVertex, section, vertexArray, sectionArray, 1.0f, 0 );

            /*BlockPath( vertexArray, sectionArray, false );*/

            // there are situations were the algorithm takes the outer ring, in that case it would find a cycle that must be discarded.

            if( ret == UOdysseyVectorGroupPaint::HASCYCLE )
            {
                //UE_LOG(LogTemp,Warning,TEXT("candidate cycle of size:%d (sections :%d)"),vertexArray.size(),sectionArray.size());

                if( /*CheckPath( vertexArray, sectionArray ) == true*/ GetNormalVector( vertexArray, sectionArray ) > 0.0f )
                {
                    //PrintCycle( vertexArray, sectionArray );
                    BlockPath( vertexArray, sectionArray, false );

                    mLoopArray.push_back( new FOdysseyVectorLoop( *this, /*iCycleID*/0, vertexArray, sectionArray ) );
                }
            }
        }
    }

    return 0;
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

void
UOdysseyVectorGroupPaint::CopyBuckets( UOdysseyVectorGroupPaint* iDestination )
{
     for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
     {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);
        ::ULIS::FVec2D& bucketCoords = bucket->GetCoords();
        BLPoint bucketWorldPosition = mWorldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );
        BLPoint destinationBucketPosition = iDestination->mInverseWorldMatrix.mapPoint( bucketWorldPosition );
        FOdysseyVectorBucket *bucketCopy = new FOdysseyVectorBucket( *iDestination, 0.0f, 0.0f );

        bucket->Copy( bucketCopy );

        bucketCopy->SetCoords( destinationBucketPosition.x, destinationBucketPosition.y );

        iDestination->AddBucket( bucketCopy );
     }   
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

void
UOdysseyVectorGroupPaint::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == "Tolerance" )
    {
        FindCycles();
    }
/*
    if ( iPropertyName == "IsActivated" )
        IsActivatedChanged();
    if ( iPropertyName == "IsLocked" )
        IsLockedChanged();
    if ( iPropertyName == "IsExpanded" )
        IsExpandedChanged();
    if ( iPropertyName == "Parent" )
        ParentChanged();
    if ( iPropertyName == "Children" )
        ChildrenChanged();
*/
}
