#include "OdysseyVectorGroupPaint.h"

// MUST be even number
#define EDGESUBSAMPLES 8

FOdysseyVectorGroupPaint::~FOdysseyVectorGroupPaint()
{
    //ClearCycles();
}

FOdysseyVectorGroupPaint::FOdysseyVectorGroupPaint()
{
    mDependsOnChildren = true;

    mIntersectionVertexArray.reserve( 60 );

    mGroupPaintParam.Tolerance = 0.0f;
    mGroupPaintParam.Realtime = false;
}

bool
FOdysseyVectorGroupPaint::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorGroup::HasBaseClass( iBaseClassID );
}

uint32
FOdysseyVectorGroupPaint::GetType()
{
    return FOdysseyVectorObject::VECTORGROUPPAINTTYPE;
}

void
FOdysseyVectorGroupPaint::Init( std::string iName )
{
    SetName( iName );
}

std::list<FOdysseyVectorBucket*>&
FOdysseyVectorGroupPaint::GetBucketList()
{
    return mBucketList;
}

void
FOdysseyVectorGroupPaint::Colorize()
{
    // reset color for all cycles first
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mLoopArray[i];

        cycle->SetBucket( nullptr );
    }

    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        ApplyBucket( bucket );
    }
}

void
FOdysseyVectorGroupPaint::ApplyBucket( FOdysseyVectorBucket* iBucket )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mLoopArray[i];

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
FOdysseyVectorGroupPaint::PickBucket( double iWorldX, double iWorldY )
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        if ( bucket->Pick( iWorldX, iWorldY ) )
        {
            return bucket;
        }
    }

    return nullptr;
}

FOdysseyVectorCycle*
FOdysseyVectorGroupPaint::PickCycle( double iX, double iY )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mLoopArray[i];

        // TODO: Bounding volume for cycles for faster search
        if( cycle->HitTest( iX, iY ) == true )
        {
            return cycle;
        }
    }

    return nullptr;
}

void
FOdysseyVectorGroupPaint::OnChildTransform( FOdysseyVectorObject* iChild )
{
    // we don't check the type of the object. Normally they should be
    // all of base type PathCubic, otherwise there is a bug somewhere.
    //iChild->SwitchSpace( *this );
}

void
FOdysseyVectorGroupPaint::OnChildAdd( FOdysseyVectorObject* iChild )
{
    if( iChild->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
    {
        FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(iChild);

        cubicPath->SwitchSpace( *this );
        cubicPath->ResetTransform();
        cubicPath->UpdateMatrix();

        cubicPath->InvalidateAllSegments();
        //cubicPath->Update( 0 );
    }

    Invalidate();
}

void
FOdysseyVectorGroupPaint::OnChildRemove( FOdysseyVectorObject* iChild )
{
    Invalidate();
}

FOdysseyVectorBucket*
FOdysseyVectorGroupPaint::Bucket( double iX, double iY, uint8 iR, uint8 iG, uint8 iB, uint8 iA )
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
FOdysseyVectorGroupPaint::UpdateShape( uint32 iUpdateFlags )
{
    FOdysseyVectorGroup::UpdateShape( iUpdateFlags ); // updates BBox

    if(   ( mGroupPaintParam.Realtime == true  )
     || ( ( mGroupPaintParam.Realtime == false ) && ( ( iUpdateFlags & FOdysseyVectorObject::FREQUENTUPDATES ) == 0 ) ) )
    {
        FindCycles();
    }
}

void
FOdysseyVectorGroupPaint::AddBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.push_back( iBucket );

    //iBucket->SetParent( this );
}

void
FOdysseyVectorGroupPaint::RemoveBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.remove( iBucket );
}

void
FOdysseyVectorGroupPaint::DrawBuckets( ::ULIS::FRectD& iRoi,uint64 iFlags )
{
    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        bucket->Draw( iRoi, iFlags );
    }
}

void
FOdysseyVectorGroupPaint::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mLoopArray[i];

        cycle->Draw( iRoi, iFlags );
    }
}

bool
FOdysseyVectorGroupPaint::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        for( int i = 0; i < mLoopArray.size(); i++ )
        {
            FOdysseyVectorCycle *cycle = mLoopArray[i];

            if( cycle->HitTest( pt.x, pt.y ) )
            {
                return true;
            }
        }
    }

    return false;
}

uint32
FOdysseyVectorGroupPaint::IntersectSegment( FOdysseyVectorSegmentCubic& iCubicSegment
                                          , std::list<FOdysseyVectorSegment*>& cubicSegmenList
                                          , std::vector<FOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    uint32 intersectionCount = 0;

    for( std::list<FOdysseyVectorSegment*>::iterator sit = cubicSegmenList.begin(); sit != cubicSegmenList.end(); ++sit )
    {
        FOdysseyVectorSegmentCubic *intersectSegment = static_cast<FOdysseyVectorSegmentCubic*>(*sit);
        ::ULIS::FRectD intersectRect = intersectSegment->GetBoundingBox( false ) & iCubicSegment.GetBoundingBox( false );

        if( intersectRect.Area() )
        {
            intersectionCount += iCubicSegment.Intersect( *intersectSegment, mGroupPaintParam.Tolerance, iIntersectionVertexArray );
        }
    }

    return intersectionCount;
}

static void
PrintCycle( std::vector<FOdysseyVectorVertex*>& vertexArray
          , std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = sectionArray[i]->GetSegment();
        BLPoint pt0 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(0)->GetCoords(segment).x, sectionArray[i]->GetVertex(0)->GetCoords(segment).y );
        BLPoint pt1 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(1)->GetCoords(segment).x, sectionArray[i]->GetVertex(1)->GetCoords(segment).y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

uint32
FOdysseyVectorGroupPaint::FindPath( FOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation
                                  , uint32 iDepth ) // we could also use iVertexArray.size()
{
    FOdysseyVectorVertex* nextVertex = ( iSection->GetVertex(0) == iVertex ) ? iSection->GetVertex(1)
                                                                             : iSection->GetVertex(0);
    bool hasPrimary = false;
    bool hasSecondary = false;
    bool hasTertiary = false;
    uint32 ret = FOdysseyVectorGroupPaint::NOCYCLE;

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
            ret = FOdysseyVectorGroupPaint::HASCYCLE;
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
                ret = FOdysseyVectorGroupPaint::NOCYCLE;
            }
        }

        if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && secondaryNextSection )
        {
            if( secondaryNextSection->IsBlocked( nextVertex ) == false )
            {
                ret = FindPath( nextVertex, secondaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
            }
            else
            {
                ret = FOdysseyVectorGroupPaint::NOCYCLE;
            }
        }

        if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && tertiaryNextSection )
        {
            if( tertiaryNextSection->IsBlocked( nextVertex ) == false )
            {
                ret = FindPath( nextVertex, tertiaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
            }
            else
            {
                ret = FOdysseyVectorGroupPaint::NOCYCLE;
            }
        }
    }
/*
    iSection->Block( iVertex );
*/
    iSection->UnBlock( iVertex );
    iVertex->SetVisited( false );

    if( ret != FOdysseyVectorGroupPaint::HASCYCLE )
    {
        iVertexArray.pop_back();
        iSectionArray.pop_back();
    }

    return ret;
}

static void
BlockPath( std::vector<FOdysseyVectorVertex*>& iVertexArray
          , std::vector<FOdysseyVectorSection*>& iSectionArray
          , bool iIsContour )
{
    for( int i = 0; i < iSectionArray.size(); i++ )
    {
        iSectionArray[i]->Block( iVertexArray[i] );
    }
}

static double
GetNormalVector( std::vector<FOdysseyVectorVertex*>& iVertexArray
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
        ::ULIS::FVec2D startVector =  -iSectionArray[0]->GetVectorFromVertex( iVertexArray[1], false );
        ::ULIS::FVec2D endVector   =   iSectionArray[1]->GetVectorFromVertex( iVertexArray[1], false );

        z = FOdysseyVector::Cross2D( startVector, endVector );
    }

    if( iVertexArray.size() >= 3 )
    {
        for( int i = 0; i < arraySize; i++ )
        {
            int n = ( i + 1 ) % arraySize;
            FOdysseyVectorSegment* segment = iSectionArray[i]->GetSegment();
            ::ULIS::FVec2D& viCoords = iVertexArray[i]->GetCoords( segment );
            ::ULIS::FVec2D& vnCoords = iVertexArray[n]->GetCoords( segment );
            double ti = iVertexArray[i]->GetT( segment );
            double tn = iVertexArray[n]->GetT( segment );
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
FOdysseyVectorGroupPaint::MarchVertex( FOdysseyVectorVertexIntersection* iIntersectionVertex )
{
    std::list<FOdysseyVectorSection*>& sectionList = iIntersectionVertex->GetSectionList();

    //UE_LOG(LogTemp,Warning,TEXT("vertex :%f %f (sections :%d)"), iIntersectionVertex->GetCoords().x, iIntersectionVertex->GetCoords().y, iIntersectionVertex->GetSectionCount() );

    for( std::list<FOdysseyVectorSection*>::iterator pit = sectionList.begin(); pit != sectionList.end(); ++pit )
    {
        FOdysseyVectorSection *section = (*pit);
        std::vector<FOdysseyVectorVertex*> vertexArray;
        std::vector<FOdysseyVectorSection*> sectionArray;

        if( section->IsBlocked(iIntersectionVertex) == false ) {
            uint32 ret = FindPath( iIntersectionVertex, section, vertexArray, sectionArray, 1.0f, 0 );

            /*BlockPath( vertexArray, sectionArray, false );*/

            // there are situations were the algorithm takes the outer ring, in that case it would find a cycle that must be discarded.

            if( ret == FOdysseyVectorGroupPaint::HASCYCLE )
            {
                //UE_LOG(LogTemp,Warning,TEXT("candidate cycle of size:%d (sections :%d)"),vertexArray.size(),sectionArray.size());

                if( /*CheckPath( vertexArray, sectionArray ) == true*/ GetNormalVector( vertexArray, sectionArray ) > 0.0f )
                {
                    //PrintCycle( vertexArray, sectionArray );
                    BlockPath( vertexArray, sectionArray, false );

                    mLoopArray.push_back( new FOdysseyVectorCycle( *this, /*iCycleID*/0, vertexArray, sectionArray ) );
                }
            }
        }
    }

    return 0;
}

// build cycles from looping path as well
void
FOdysseyVectorGroupPaint::CheckLoops()
{
    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorObject *child = (*oit);

        if( child->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
        {
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(child);

            if( cubicPath->HasIntersections() == false )
            {
                if( cubicPath->IsLoop() )
                {
                    std::vector<FOdysseyVectorVertex*> vertexArray;
                    std::vector<FOdysseyVectorSection*> sectionArray;

                    cubicPath->ToVertexAndSectionArray( vertexArray, sectionArray );

                    if( vertexArray.size() )
                    {
                        mLoopArray.push_back( new FOdysseyVectorCycle( *this, /*iCycleID*/0, vertexArray, sectionArray ) );
                    }
                }
            }
        }
    }
}

void
FOdysseyVectorGroupPaint::FindCycles()
{
    BuildGraph();

    //UE_LOG( LogTemp, Warning, TEXT("Detection -----------------------------------------------------------") );
    //UE_LOG( LogTemp, Warning, TEXT("Intersection vertices:%d"), intersectionVertexList.size() );

    for( int i = 0; i < mIntersectionVertexArray.size(); i++ )
    {
        MarchVertex( mIntersectionVertexArray[i] );
    }

    CheckLoops();

    OrderCycles();

    Colorize();
}

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    std::list<FOdysseyVectorSegment*> cubicSegmenList;
    FOdysseyVectorSegmentCubic *cubicSegment;

    ClearCycles( cubicSegmenList );

    cubicSegment = cubicSegmenList.size() ? static_cast<FOdysseyVectorSegmentCubic*>( cubicSegmenList.back() ) : nullptr;

    while( cubicSegment )
    {
        IntersectSegment ( *cubicSegment, cubicSegmenList, mIntersectionVertexArray );

        // remove segment from list as they are tested
        cubicSegmenList.pop_back();

        cubicSegment = cubicSegmenList.size() ? static_cast<FOdysseyVectorSegmentCubic*>( cubicSegmenList.back() ) : nullptr;
    }
}

void
FOdysseyVectorGroupPaint::OrderCycles()
{
    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorCycle* cycle = mLoopArray[i];

        for( int j = 0; j < mLoopArray.size(); j++ )
        {
           FOdysseyVectorCycle* innerCycle = mLoopArray[j];

           if( cycle != innerCycle )
           {
               if( innerCycle->FitsIn( cycle ) )
               {
                   FOdysseyVectorCycle* parentCycle = innerCycle->GetParentCycle();

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
FOdysseyVectorGroupPaint::ClearCycles( std::list<FOdysseyVectorSegment*>& cubicSegmenList )
{
    bool bboxInit = false;

    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorObject *child = (*oit);

        if( child->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
        {
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(child);
            std::list<FOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();

            mBBox = ( bboxInit == false ) ? cubicPath->GetBBox( false ) : mBBox | cubicPath->GetBBox( false );

            bboxInit = true;

            for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                FOdysseyVectorSegmentCubic *cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*sit);

                cubicSegment->ClearIntersections();

                cubicSegmenList.push_back( cubicSegment );
            }
        }
    }

    for( int i = 0; i < mIntersectionVertexArray.size(); i++ )
    {
        delete mIntersectionVertexArray[i];
    }

    mIntersectionVertexArray.clear();

    for( int i = 0; i < mLoopArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mLoopArray[i];

        delete cycle;
    }

    mLoopArray.clear();
}

void
FOdysseyVectorGroupPaint::CopyBuckets( FOdysseyVectorGroupPaint* iDestination )
{
     for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
     {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);
        ::ULIS::FVec2D bucketCoords = bucket->GetCoords();
        BLPoint bucketWorldPosition = mWorldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );
        BLPoint destinationBucketPosition = iDestination->mInverseWorldMatrix.mapPoint( bucketWorldPosition );
        FOdysseyVectorBucket *bucketCopy = new FOdysseyVectorBucket( *iDestination, 0.0f, 0.0f );

        bucket->Copy( bucketCopy );

        bucketCopy->SetCoords( destinationBucketPosition.x, destinationBucketPosition.y );

        iDestination->AddBucket( bucketCopy );
     }   
}

FOdysseyVectorObject*
FOdysseyVectorGroupPaint::CopyShape()
{
    return new FOdysseyVectorGroupPaint();
}

// TODO : bounding box segments.
// TODO: nested cycles.
// TODO: convert path to the group coordinates.
// check number of cycles / valence


FOdysseyVectorHandleBucket*
FOdysseyVectorGroupPaint::PickBucketHandle( double iX, double iY )
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
FOdysseyVectorGroupPaint::PropertyChanged(const FName& iPropertyName)
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
