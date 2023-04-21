#include "OdysseyVectorGroupPaint.h"

// MUST be even number
#define EDGESUBSAMPLES 8

static double
GetNormalVector( std::vector<FOdysseyVectorVertex*>& iVertexArray
               , std::vector<FOdysseyVectorSection*>& iSectionArray );
static void
BlockPath( std::vector<FOdysseyVectorVertex*>& iVertexArray
         , std::vector<FOdysseyVectorSection*>& iSectionArray );

FOdysseyVectorGroupPaint::~FOdysseyVectorGroupPaint()
{
    //ClearCycles();
}

FOdysseyVectorGroupPaint::FOdysseyVectorGroupPaint( std::string iName )
{
    SetName( iName );

    mDependsOnChildren = true;

    mIntersectionArray.reserve( 60 );

    mGroupPaintParam.Tolerance = 0.0f;
    mGroupPaintParam.Realtime = false;
    mGroupPaintParam.Wireframe = false;
    mGroupPaintParam.WireframeColor = FColor( 255, 255, 255, 255 );
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

std::list<FOdysseyVectorBucket*>&
FOdysseyVectorGroupPaint::GetBucketList()
{
    return mBucketList;
}

void
FOdysseyVectorGroupPaint::Colorize()
{
    // reset color for all cycles first
    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

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
    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

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
    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

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
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

    path->SwitchSpace( *this );
    path->ResetTransform();
    path->UpdateMatrix( false );// pass false to prevent loop
    path->InvalidateAllSegments();
}

void
FOdysseyVectorGroupPaint::TransferChild( FOdysseyVectorObject* iFosterChild )
{
    FOdysseyVectorGroup::TransferChild( iFosterChild );

    // Unsure this whole code block is needed, as OnChildTransform does the job.
    if( iFosterChild->HasBaseClass( FOdysseyVectorPathCubic::StaticClass() ) )
    {
        FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(iFosterChild);

        cubicPath->SwitchSpace( *this );
        cubicPath->ResetTransform();
        cubicPath->UpdateMatrix();

        // Note: InvalidateAllSegments() will invalidate the paint group as well
        cubicPath->InvalidateAllSegments();
        //cubicPath->Update( 0 );
    }
}

void
FOdysseyVectorGroupPaint::Invalidate()
{
    std::list<FOdysseyVectorSegment*> cubicSegmenList;

    FOdysseyVectorObject::Invalidate();

    //Clear( cubicSegmenList );
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
FOdysseyVectorGroupPaint::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    uint64 extraFlags = 0;

    extraFlags |= ( ( mGroupPaintParam.Wireframe ) ? FOdysseyVectorObject::DRAWSTRUCTURE : 0 );

    FOdysseyVectorObject::Draw( iRoi, iFlags | extraFlags );
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
    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

        cycle->Draw( iRoi, iFlags );
    }
}

bool
FOdysseyVectorGroupPaint::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        for( int i = 0; i < mCycleArray.size(); i++ )
        {
            FOdysseyVectorCycle *cycle = mCycleArray[i];

            if( cycle->HitTest( pt.x, pt.y ) )
            {
                return true;
            }
        }
    }

    return false;
}

uint32
FOdysseyVectorGroupPaint::IntersectSegment( FOdysseyVectorSegmentCubic* iCubicSegment
                                          , std::list<FOdysseyVectorSegment*>& cubicSegmenList
                                          , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    uint32 intersectionCount = 0;

    for( std::list<FOdysseyVectorSegment*>::iterator sit = cubicSegmenList.begin(); sit != cubicSegmenList.end(); ++sit )
    {
        FOdysseyVectorSegmentCubic *intersectSegment = static_cast<FOdysseyVectorSegmentCubic*>(*sit);
        ::ULIS::FRectD intersectRect = intersectSegment->GetBoundingBox( false ) & iCubicSegment->GetBoundingBox( false );

        if( intersectRect.Area() )
        {
            intersectionCount += iCubicSegment->Intersect( *intersectSegment
                                                          , mGroupPaintParam.Tolerance
                                                          , iIntersectionArray );
        }
    }

    return intersectionCount;
}

static void
PrintVertex( FOdysseyVectorVertex* iVertex )
{
    FOdysseyVectorSegment* segment = iVertex->GetFirstSegment();
    BLPoint pt = segment->GetPath()->GetWorldMatrix().mapPoint( iVertex->GetCoords().x, iVertex->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Vertex: [x:%f y:%f]"), pt.x, pt.y);
}

static void
PrintSection( FOdysseyVectorSection* iSection)
{
    FOdysseyVectorSegment* segment = iSection->GetSegment();
    BLPoint pt0 = segment->GetPath()->GetWorldMatrix().mapPoint( iSection->GetVertex(0)->GetCoords().x, iSection->GetVertex(0)->GetCoords().y );
    BLPoint pt1 = segment->GetPath()->GetWorldMatrix().mapPoint( iSection->GetVertex(1)->GetCoords().x, iSection->GetVertex(1)->GetCoords().y );
    BLPoint segpt0 = segment->GetPath()->GetWorldMatrix().mapPoint( segment->GetVertex(0)->GetCoords().x, segment->GetVertex(0)->GetCoords().y );
    BLPoint segpt1 = segment->GetPath()->GetWorldMatrix().mapPoint( segment->GetVertex(1)->GetCoords().x, segment->GetVertex(1)->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Section: [x:%f y:%f] -- [x:%f y:%f]/segment[x:%f y:%f] -- [x:%f y:%f]"), pt0.x, pt0.y, pt1.x, pt1.y, segpt0.x, segpt0.y, segpt1.x, segpt1.y );
}

static void
PrintCycle( std::vector<FOdysseyVectorVertex*>& vertexArray
          , std::vector<FOdysseyVectorSection*>& sectionArray)
{
    UE_LOG(LogTemp,Warning,TEXT("Array size: %d"), vertexArray.size() );

    for( int i = 0; i < vertexArray.size(); i++ )
    {
        FOdysseyVectorSegment* segment = sectionArray[i]->GetSegment();
        BLPoint pt0 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(0)->GetCoords().x, sectionArray[i]->GetVertex(0)->GetCoords().y );
        BLPoint pt1 = segment->GetPath()->GetWorldMatrix().mapPoint( sectionArray[i]->GetVertex(1)->GetCoords().x, sectionArray[i]->GetVertex(1)->GetCoords().y );

        UE_LOG(LogTemp,Warning,TEXT("Node: vertex:%d section:%d (%d[x:%f y:%f] -- %d[x:%f y:%f])"), vertexArray[i], sectionArray[i], sectionArray[i]->GetVertex(0), pt0.x, pt0.y, sectionArray[i]->GetVertex(1), pt1.x, pt1.y );
    }
}

/*
static FOdysseyVectorSection*
NextSection( FOdysseyVectorVertex* iNextVertex, FOdysseyVectorSection* iSection )
{
    FOdysseyVectorSection* primaryNextSection = iNextVertex->GetCycleNextSection( iSection, 1.0f );

    if( primaryNextSection && ( primaryNextSection->IsBlocked( iNextVertex ) == false ) )
    {
        return primaryNextSection;
    }

    FOdysseyVectorSection* secondaryNextSection = iNextVertex->GetOtherSection( iSection );

    if( secondaryNextSection && ( secondaryNextSection->IsBlocked( iNextVertex ) == false ) )
    {
        return secondaryNextSection;
    }

    FOdysseyVectorSection* tertiaryNextSection = iNextVertex->GetCycleNextSection( iSection, -1.0f );

    if( tertiaryNextSection && ( tertiaryNextSection->IsBlocked( iNextVertex ) == false ) )
    {
        return tertiaryNextSection;
    }

    return nullptr;
}
*/
uint32
FOdysseyVectorGroupPaint::FindPath( FOdysseyVectorIntersection* iInitiatorIntersection
                                  , FOdysseyVectorSection* iReturnSection
                                  , FOdysseyVectorVertex* iVertex
                                  , FOdysseyVectorSection* iSection
                                  , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                  , std::vector<FOdysseyVectorSection*>& iSectionArray
                                  , double iOrientation
                                  , uint32 iDepth ) // we could also use iVertexArray.size()
{
    FOdysseyVectorVertex* nextVertex = ( iSection->GetVertex(0) == iVertex ) ? iSection->GetVertex(1)
                                                                             : iSection->GetVertex(0);
    ::ULIS::FVec2D sectionVector = -iSection->GetVectorFromVertex( nextVertex, false, false );

    bool hasPrimary = false;
    bool hasSecondary = false;
    bool hasTertiary = false;
    uint32 ret = FOdysseyVectorGroupPaint::NOCYCLE;
    static int i;

    iSectionArray.push_back( iSection );

    iVertexArray.push_back( iVertex );
    //iVertex->SetVisited( true );
    iSection->Block( iVertex );

if(i==4)
        UE_LOG(LogTemp,Warning,TEXT("lets break"));

    UE_LOG(LogTemp,Warning,TEXT("Exploring section:----%d"),i++ );
    PrintSection( iSection );

    if( ( iInitiatorIntersection->HasVertex( static_cast<FOdysseyVectorVertexIntersection*>(nextVertex) ) )// cycle detected
    && ( ( ( iReturnSection->IsRemoved() == false ) && ( iReturnSection == iSection ) )
           || ( iReturnSection->IsRemoved() == true ) ) )
    {
        UE_LOG(LogTemp,Warning,TEXT("cycle detected: %d"), iReturnSection->IsRemoved() );

        PrintCycle( iVertexArray, iSectionArray );

        //UE_LOG(LogTemp,Warning,TEXT("front detected") );
            if( GetNormalVector( iVertexArray, iSectionArray ) > 0.0f )
            {
        //UE_LOG(LogTemp,Warning,TEXT("normal detected") );
                //PrintCycle( iVertexArray, iSectionArray );
                /*BlockPath( vertexArray, sectionArray, false );*/

                mCycleArray.push_back( new FOdysseyVectorCycle( *this, /*iCycleID*/0, iVertexArray, iSectionArray ) );
            }

        /*if( nextVertex != iVertexArray.front() )
        {
            BlockPath( cycleVertexArray, cycleSectionArray );
        }*/

        ret = FOdysseyVectorGroupPaint::HASCYCLE;
    }
    else
    {
        if( nextVertex->GetClass() == FOdysseyVectorVertexCubic::StaticClass() )
        {
            FOdysseyVectorSection* primaryNextSection = nextVertex->GetOtherSection( iSection, false );

            if( primaryNextSection )
            {
        UE_LOG(LogTemp,Warning,TEXT("primary") );
                if( primaryNextSection->IsBlocked( nextVertex ) == false )
                {
                    ret = FindPath( iInitiatorIntersection, iReturnSection, nextVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }
        }

        if( nextVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
        {
            FOdysseyVectorVertexIntersection* nextIntersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>( nextVertex );
            FOdysseyVectorVertexIntersection* nextPartnerVertex = nextIntersectionVertex->GetPartner();
            FOdysseyVectorSection* primaryNextSection = nextPartnerVertex->GetCycleNextSection( sectionVector, 1.0f );
            FOdysseyVectorSection* secondaryNextSection = nextIntersectionVertex->GetOtherSection( iSection, true );
            FOdysseyVectorSection* tertiaryNextSection = nextPartnerVertex->GetCycleNextSection( sectionVector, -1.0f );

            if( primaryNextSection )
            {
        UE_LOG(LogTemp,Warning,TEXT("primary") );
                if( primaryNextSection->IsBlocked( nextPartnerVertex ) == false )
                {
                    ret = FindPath( iInitiatorIntersection, iReturnSection, nextPartnerVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }

            if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && secondaryNextSection )
            {
        UE_LOG(LogTemp,Warning,TEXT("secondary") );
                if( secondaryNextSection->IsBlocked( nextIntersectionVertex ) == false )
                {
                    ret = FindPath( iInitiatorIntersection, iReturnSection, nextIntersectionVertex, secondaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }

            if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && tertiaryNextSection )
            {
       UE_LOG(LogTemp,Warning,TEXT("tertiary") );
                if( tertiaryNextSection->IsBlocked( nextPartnerVertex ) == false )
                {
                    ret = FindPath( iInitiatorIntersection, iReturnSection, nextPartnerVertex, tertiaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }
        }
    }
/*
    iSection->Block( iVertex );
*/
    //iSection->UnBlock( iVertex ); // never unblock. A section is never visited more than once in one way
    //iVertex->SetVisited( false );

    /*if( ret != FOdysseyVectorGroupPaint::HASCYCLE )
    {*/
        iVertexArray.pop_back();
        iSectionArray.pop_back();
    /*}*/

    return ret;
}

static double
GetNormalVector( std::vector<FOdysseyVectorVertex*>& iVertexArray
               , std::vector<FOdysseyVectorSection*>& iSectionArray )
{
    double z = 0;
    int32 arraySize = iVertexArray.size();

    for( int i = 0; i < arraySize; i++ )
    {
        int n = ( i + 1 ) % arraySize;
        FOdysseyVectorSegment* segment = iSectionArray[i]->GetSegment();
        FOdysseyVectorVertex* vertexi = iVertexArray[i];
        FOdysseyVectorVertex* vertexn = iVertexArray[n];

        if( vertexn->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
        {
            FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(vertexn);

            vertexn = intersectionVertex->GetPartner();
        }

        ::ULIS::FVec2D& viCoords = vertexi->GetCoords();
        ::ULIS::FVec2D& vnCoords = vertexn->GetCoords();

        if( segment )
        {
            double ti = vertexi->GetT( segment );
            double tn = vertexn->GetT( segment );
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
        }
        else
        {
            z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
        }

// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
// Newell's method
        //z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
    }

    //UE_LOG(LogTemp,Warning,TEXT("Normal Z:%f"), z);

    return z;
}

uint32
FOdysseyVectorGroupPaint::MarchIntersection( FOdysseyVectorIntersection* iIntersection )
{
    std::vector<FExplorationPair>& explorationPairs = iIntersection->GetExplorationPairs();

    //UE_LOG(LogTemp,Warning,TEXT("vertex :%f %f (sections :%d)"), iIntersectionVertex->GetCoords().x, iIntersectionVertex->GetCoords().y, iIntersectionVertex->GetSectionCount() );

    //PrintVertex( iIntersection );

    for( uint32 i = 0; i < explorationPairs.size(); i++ )
    {
        ///UE_LOG(LogTemp,Warning,TEXT("<--- Return section:"));
        //PrintSection( explorationPairs[i].returnSection );
        //UE_LOG(LogTemp,Warning,TEXT("---> Depart section:"));
       // PrintSection( explorationPairs[i].departSection );

      if( explorationPairs[i].departSection )
        if( ( explorationPairs[i].returnSection->IsRemoved() == false )
         && ( explorationPairs[i].departSection->IsRemoved() == false ) )
        {
            FOdysseyVectorVertex* partnerVertex = explorationPairs[i].intersectionVertex->GetPartner();

            if( explorationPairs[i].departSection->IsBlocked( partnerVertex ) == false )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                uint32 ret = FindPath( iIntersection
                                     , explorationPairs[i].returnSection
                                     , partnerVertex // is on departSection
                                     , explorationPairs[i].departSection
                                     , vertexArray
                                     , sectionArray
                                     , 1.0f
                                     , 0 );
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
                        mCycleArray.push_back( new FOdysseyVectorCycle( *this, /*iCycleID*/0, vertexArray, sectionArray ) );
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

    // Build exploration pair before simplification
    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {

        mIntersectionArray[i]->BuildExplorationPairs();
    }

    SimplifyGraph();

    //UE_LOG( LogTemp, Warning, TEXT("Detection -----------------------------------------------------------") );
    //UE_LOG( LogTemp, Warning, TEXT("Intersection vertices:%d"), intersectionVertexList.size() );

    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        //UE_LOG( LogTemp, Warning, TEXT("sectionCount = %d\n"), mIntersectionArray[i]->GetVertex(0)->GetSectionCount() );

        MarchIntersection( mIntersectionArray[i] );
    }

    CheckLoops();

    OrderCycles();

    Colorize();
}

static void //uint32
CreateNearIntersection( FOdysseyVectorVertex *iVertex
                      , std::vector<FOdysseyVectorVertexIntersection*>& iIntersectionVertexArray )
{
    FOdysseyVectorSegment *nearestSegment = iVertex->GetNearestSegment();
    //uint32 intersectionCount = 0;

    if( nearestSegment )
    {
        double nearestSegmentT = iVertex->GetNearestSegmentT();
        FOdysseyVectorVertex* nearestVertex;

        //FOdysseyVectorSegmentCubic cubicSegment = FOdysseyVectorSegmentCubic::New()

        if( ( nearestSegmentT != 0.0f ) && ( nearestSegmentT != 1.0f ) )
        {
            ::ULIS::FVec2D nearestVertexAt = nearestSegment->GetPointAt( nearestSegmentT );
            FOdysseyVectorVertexIntersection* intersectionVertex = new FOdysseyVectorVertexIntersection( nearestVertexAt.x, nearestVertexAt.y, false );

            intersectionVertex->AddSegment ( nearestSegment/*, nearestSegmentT*/ );
            nearestSegment->AddIntersection ( intersectionVertex );

            iIntersectionVertexArray.push_back( intersectionVertex );

            nearestVertex = intersectionVertex;
        }
        else
        {
            nearestVertex = nearestSegment->GetVertex((int)nearestSegmentT);
        }

        nearestSegment->AddSection( new FOdysseyVectorSection ( nullptr, iVertex, nearestVertex ) );
    }
/*
    if( vertex0->GetNearestSegment() )
    {
        FOdysseyVectorSegment *nearestSegment = vertex0->GetNearestSegment();
        ::ULIS::FVec2D isxAt[2] = { vertex0->GetCoords()
                                  , nearestSegment->GetPointAt( vertex0->GetNearestSegmentT() ) };

        FOdysseyVectorVertexIntersection* intersectionVertex[2] = { new FOdysseyVectorVertexIntersection( isxAt[0].x, isxAt[0].y )
                                                                  , new FOdysseyVectorVertexIntersection( isxAt[1].x, isxAt[1].y ) };

        iIntersectionVertexArray.push_back( intersectionVertex[0] );
        iIntersectionVertexArray.push_back( intersectionVertex[1] );

        // AddSegment() MUST be called before AddIntersection because AddIntersection uses the value of t that is stored by AddSegment()
        intersectionVertex[0]->AddSegment (       iSegment, vertex0->GetT( iSegment ) );
        intersectionVertex[1]->AddSegment ( nearestSegment, vertex0->GetNearestSegmentT() );

        iSegment->AddIntersection ( intersectionVertex[0] );
        nearestSegment->AddIntersection ( intersectionVertex[1] );

        iSegment->AddSection ( new FOdysseyVectorSection ( nullptr
                                                         , intersectionVertex[0]
                                                         , intersectionVertex[1] ) );

        intersectionCount++;
    }

    if( vertex1->GetNearestSegment() )
    {
        FOdysseyVectorSegment *nearestSegment = vertex1->GetNearestSegment();
        ::ULIS::FVec2D isxAt[2] = { vertex1->GetCoords()
                                  , nearestSegment->GetPointAt( vertex1->GetNearestSegmentT() ) };
        FOdysseyVectorVertexIntersection* intersectionVertex[2] = { new FOdysseyVectorVertexIntersection( isxAt[0].x, isxAt[0].y )
                                                                  , new FOdysseyVectorVertexIntersection( isxAt[1].x, isxAt[1].y ) };

        iIntersectionVertexArray.push_back( intersectionVertex[0] );
        iIntersectionVertexArray.push_back( intersectionVertex[1] );

        // AddSegment() MUST be called before AddIntersection because AddIntersection uses the value of t that is stored by AddSegment()
        intersectionVertex[0]->AddSegment (       iSegment, vertex1->GetT( iSegment ) );
        intersectionVertex[1]->AddSegment ( nearestSegment, vertex1->GetNearestSegmentT() );

        iSegment->AddIntersection ( intersectionVertex[0] );
        nearestSegment->AddIntersection ( intersectionVertex[1] );

        iSegment->AddSection ( new FOdysseyVectorSection ( nullptr
                                                         , intersectionVertex[0]
                                                         , intersectionVertex[1] ) );

        intersectionCount++;
    }
*/
    //return intersectionCount;
}

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    std::list<FOdysseyVectorSegment*> segmentList;
    FOdysseyVectorSegmentCubic *cubicSegment;

    Clear();

    segmentList = mSegmentList;

    cubicSegment = segmentList.size() ? static_cast<FOdysseyVectorSegmentCubic*>( segmentList.back() ) : nullptr;

    while( cubicSegment )
    {
        IntersectSegment ( cubicSegment, segmentList, mIntersectionArray );

        // retrieve near-intersection
        //CreateNearIntersection( cubicSegment->GetVertex(0), mIntersectionVertexArray );
        //CreateNearIntersection( cubicSegment->GetVertex(1), mIntersectionVertexArray );

        // remove segment from list as they are tested
        segmentList.pop_back();

        cubicSegment = segmentList.size() ? static_cast<FOdysseyVectorSegmentCubic*>( segmentList.back() ) : nullptr;
    }
}

void
FOdysseyVectorGroupPaint::OrderCycles()
{
    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle* cycle = mCycleArray[i];

        for( int j = 0; j < mCycleArray.size(); j++ )
        {
           FOdysseyVectorCycle* innerCycle = mCycleArray[j];

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
FOdysseyVectorGroupPaint::SimplifyGraph()
{
    bool keepSimplifying;

    for( std::list<FOdysseyVectorObject*>::iterator cit = mChildrenList.begin(); cit != mChildrenList.end(); ++cit )
    {
        FOdysseyVectorObject *child = (*cit);

        if( child->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(child);
            std::list<FOdysseyVectorSegment*>& segmentList = cubicPath->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
            {
                FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*it);
                std::list<FOdysseyVectorSection*>& segmentSectionList = segment->GetSectionList();

                mSectionList.insert( mSectionList.end(), segmentSectionList.begin(), segmentSectionList.end() );
            }
        }
    }

    do
    {
        keepSimplifying = false;

        for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
        {
            FOdysseyVectorSection *section = static_cast<FOdysseyVectorSection*>(*it);

            if( section->IsRemoved() == false )
            {
                if( section->GetSegment() )
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
                        section->SetRemoved();
                    }
                }
            }
        }
    } while ( keepSimplifying );
}


void
FOdysseyVectorGroupPaint::Clear()
{
    bool bboxInit = false;

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->ClearIntersections();
    }

    mSegmentList.clear();

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection *section = (*it);

        //if( section != section->GetSegment()->GetDefaultSection() )
        delete section;
    }

    mSectionList.clear();

    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorObject *child = (*oit);

        if( child->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            std::list<FOdysseyVectorSegment*>& pathSegmentList = path->GetSegmentList();
            //std::list<FOdysseyVectorVertex*>& pathVertexList = path->GetVertexList();

            mBBox = ( bboxInit == false ) ? path->GetBBox( false ) : mBBox | path->GetBBox( false );

            bboxInit = true;

            for( std::list<FOdysseyVectorSegment*>::iterator sit = pathSegmentList.begin(); sit != pathSegmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*sit);

                segment->GetVertex(0)->SetNearestSegment( nullptr, mGroupPaintParam.Tolerance/*DBL_MAX*/, 0.0f );
                segment->GetVertex(1)->SetNearestSegment( nullptr, mGroupPaintParam.Tolerance/*DBL_MAX*/, 0.0f );

                segment->AddSection ( new FOdysseyVectorSection ( segment
                                                                , segment->GetVertex(0)
                                                                , segment->GetVertex(1) ) );

                mSegmentList.push_back( segment );
            }
        }
    }

    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        delete mIntersectionArray[i];
    }

    mIntersectionArray.clear();

    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

        delete cycle;
    }

    mCycleArray.clear();
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
    return new FOdysseyVectorGroupPaint( "Paint Group Copy" );
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
