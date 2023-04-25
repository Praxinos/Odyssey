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

    //mGapSegmentBuffer.reserve( 200 );
    //mSectionBuffer.reserve( 200 );
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
FOdysseyVectorGroupPaint::PropagateBuckets()
{
    std::vector<FOdysseyVectorCycle*> cycleArray = mCycleArray; // work on a copy
    std::vector<FOdysseyVectorCycle*> oContaminatedCycleArray;

    oContaminatedCycleArray.reserve( cycleArray.size() );

    while( cycleArray.size() )
    {
        for( int i = 0; i < cycleArray.size(); i++ )
        {
            FOdysseyVectorCycle *cycle = cycleArray[i];
            FOdysseyVectorBucket* bucket = cycle->GetBucket();

            if( cycle->IsPropagated() == false )
            {
                cycle->PropagateBucket( oContaminatedCycleArray );
            }
        }

        cycleArray = oContaminatedCycleArray;

        oContaminatedCycleArray.clear();
    }
}

void
FOdysseyVectorGroupPaint::Colorize()
{
    // reset color for all cycles first
    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

        cycle->SetBucket( nullptr );
        cycle->SetPropagated( false );
    }

    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        ApplyBucket( bucket );
    }

    PropagateBuckets();
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
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iFosterChild);

    iFosterChild->GetParent()->RemoveChild( iFosterChild );

    AppendChild( iFosterChild );

    OnChildTransform( path );
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
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();

    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

        cycle->Draw( iRoi, iFlags );
    }

    if( iFlags & FOdysseyVectorObject::DRAWSTRUCTURE )
    {
        blctx->save();
        blctx->resetMatrix();
        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );

        for( int i = 0; i < mGapSegmentBuffer.size(); i++ )
        {
            mGapSegmentBuffer[i].DrawStructure( this, iRoi, true );
        }

        blctx->restore();
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

static void
CreateVertexGapSegment( FOdysseyVectorVertex* iVertex
                      , std::vector<FOdysseyVectorSection>& iSectionBuffer
                      , std::vector<FOdysseyVectorSegmentCubic>& iGapSegmentBuffer )
{
    FOdysseyVectorVertex* nearestVertex = iVertex->GetNearestVertex();

    if( nearestVertex )
    {
        uint32 sectionCount = iSectionBuffer.size();
        uint32 gapCount = iGapSegmentBuffer.size();

        // if both vertices are regular vertices, only the one with the highest ptr has the right to create the segment
        if( ( ( nearestVertex->GetNearestVertex() == iVertex )  && ( iVertex > nearestVertex ) )
        ||    ( nearestVertex->GetNearestVertex() != iVertex )
        // or if the nearest vertex is an intersection vertex.
        ||    ( nearestVertex->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() ) )
        {
            iGapSegmentBuffer.emplace_back();
            iGapSegmentBuffer[gapCount].Init( nullptr, nearestVertex, iVertex );

            iSectionBuffer.emplace_back();
            iSectionBuffer[sectionCount].Init( &iGapSegmentBuffer[gapCount], nearestVertex, iVertex );
            iSectionBuffer[sectionCount].Link();

            ::ULIS::FVec2D delta = iVertex->GetCoords() - nearestVertex->GetCoords();
            double length = delta.Distance() * 0.33f;

            if( iVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
            {
                ::ULIS::FVec2D vertexVector = iVertex->GetFirstSegment()->GetVectorFromVertex( iVertex, true );

                iGapSegmentBuffer[gapCount].GetHandle(1)->Set( iVertex->GetCoords() - ( vertexVector * length ) );
            }

            if( nearestVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
            {
                ::ULIS::FVec2D vertexVector = nearestVertex->GetFirstSegment()->GetVectorFromVertex( nearestVertex, true );

                iGapSegmentBuffer[gapCount].GetHandle(0)->Set( nearestVertex->GetCoords() - ( vertexVector * length ) );
            }

            iGapSegmentBuffer[gapCount].Update();
        }
    }
}

static void
CreateSegmentSections( FOdysseyVectorSegment* iSegment
                     , std::vector<FOdysseyVectorSection>& iSectionBuffer )
{
    std::list<FOdysseyVectorVertexIntersection*>& intersectionVertexList = iSegment->GetIntersectionVertexList();
    std::vector<FOdysseyVectorVertex*> vertertexArray;
    FOdysseyVectorVertex* sectionVertex0;

    vertertexArray.reserve( 2 + intersectionVertexList.size() );

    // retrieve intersection vertices + segment end points from lower t value to higher t value
    iSegment->GetAllVertices( vertertexArray );

    // Create sections
    sectionVertex0 = vertertexArray[0];

    for( int i = 1; i < vertertexArray.size(); i++ )
    {
        FOdysseyVectorVertex* sectionVertex1 = vertertexArray[i];
        uint32 sectionCount = iSectionBuffer.size();

        iSectionBuffer.emplace_back();
        // creates topology
        iSectionBuffer[sectionCount].Init( iSegment, sectionVertex0, sectionVertex1 );
        iSectionBuffer[sectionCount].Link();

        sectionVertex0 = sectionVertex1;
    }
}

static void
CreatePathSections( FOdysseyVectorPath* iPath
                  , std::vector<FOdysseyVectorSection>& iSectionBuffer
                  , std::vector<FOdysseyVectorSegmentCubic>& iGapSegmentBuffer )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*it);
        FOdysseyVectorVertex* vertex0 = segment->GetVertex( 0 );
        FOdysseyVectorVertex* vertex1 = segment->GetVertex( 1 );

        CreateSegmentSections( segment, iSectionBuffer );

        //TODO::Possible optimization: call only if nearestVertex exists
        CreateVertexGapSegment( vertex0, iSectionBuffer, iGapSegmentBuffer );
        CreateVertexGapSegment( vertex1, iSectionBuffer, iGapSegmentBuffer );
    }
}

uint32
FOdysseyVectorGroupPaint::IntersectSegment( FOdysseyVectorSegment* iSegment
                                          , std::list<FOdysseyVectorSegment*>& iSegmenList
                                          , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    uint32 intersectionCount = 0;

    for( std::list<FOdysseyVectorSegment*>::iterator sit = iSegmenList.begin(); sit != iSegmenList.end(); ++sit )
    {
        FOdysseyVectorSegmentCubic *intersectSegment = static_cast<FOdysseyVectorSegmentCubic*>(*sit);

        if( intersectSegment->GetPaintingCode() != mPaintingCode )
        {
            ::ULIS::FRectD intersectRect = intersectSegment->GetBoundingBox( false ) & iSegment->GetBoundingBox( false );

            if( intersectRect.Area() )
            {
                intersectionCount += iSegment->Intersect( intersectSegment
                                                        , mGroupPaintParam.Tolerance
                                                        , iIntersectionArray );
            }
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
FOdysseyVectorGroupPaint::FindPath( FOdysseyVectorSection* iReturnSection
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
    bool isLoop = false;

    iSectionArray.push_back( iSection );
    iVertexArray.push_back( iVertex );
    iSection->Block( iVertex );

    //UE_LOG(LogTemp, Warning, TEXT("exploring section:") );
    //PrintSection( iSection );
    // loop checking if initiator is of type 
    if( iVertexArray[0]->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(iVertexArray[0]);

        isLoop = intersectionVertex->GetIntersection()->HasVertex( static_cast<FOdysseyVectorVertexIntersection*>(nextVertex) );
    }

    if( iVertexArray[0]->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        isLoop = ( iVertexArray[0] == nextVertex );
    }

    if( ( isLoop == true )// cycle detected
    && ( ( ( iReturnSection->IsLinked() == true ) && ( iReturnSection == iSection ) ) // 1 return path accepted
           || ( iReturnSection->IsLinked() == false ) ) ) // any return path accepted
    {
        //UE_LOG(LogTemp,Warning,TEXT("cycle detected") );

        //PrintCycle( iVertexArray, iSectionArray );

        if( GetNormalVector( iVertexArray, iSectionArray ) > 0.0f )
        {
        //UE_LOG(LogTemp,Warning,TEXT("normal detected") );

            mCycleArray.push_back( new FOdysseyVectorCycle( *this, /*iCycleID*/0, iVertexArray, iSectionArray ) );
        }

        ret = FOdysseyVectorGroupPaint::HASCYCLE;
    }
    else
    {
        if( nextVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorSection* primaryNextSection = nextVertex->GetOtherSection( iSection, false );

            if( primaryNextSection )
            {
        //UE_LOG(LogTemp,Warning,TEXT("primary") );
                if( primaryNextSection->IsBlocked( nextVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
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
        //UE_LOG(LogTemp,Warning,TEXT("primary") );
                if( primaryNextSection->IsBlocked( nextPartnerVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextPartnerVertex, primaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }

            if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && secondaryNextSection )
            {
        //UE_LOG(LogTemp,Warning,TEXT("secondary") );
                if( secondaryNextSection->IsBlocked( nextIntersectionVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextIntersectionVertex, secondaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }

            if( ( ret == FOdysseyVectorGroupPaint::NOCYCLE ) && tertiaryNextSection )
            {
       //UE_LOG(LogTemp,Warning,TEXT("tertiary") );
                if( tertiaryNextSection->IsBlocked( nextPartnerVertex ) == false )
                {
                    ret = FindPath( iReturnSection, nextPartnerVertex, tertiaryNextSection, iVertexArray, iSectionArray, iOrientation, iDepth + 1 );
                }
                else
                {
                    ret = FOdysseyVectorGroupPaint::BLOCKED;
                }
            }
        }
    }

    iVertexArray.pop_back();
    iSectionArray.pop_back();

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

            if( ( iSectionArray[i]->GetSegment() != iSectionArray[n]->GetSegment() )
             || ( intersectionVertex->SelfIntersects() == true ) )
            {
                vertexn = intersectionVertex->GetPartner();
            }
        }
/*
if( ( iSectionArray[i]->GetVertex(0) != vertexi ) && ( iSectionArray[i]->GetVertex(0) != vertexn ) )
    UE_LOG(LogTemp,Warning,TEXT("Might be an issue"));
if( ( iSectionArray[i]->GetVertex(1) != vertexi ) && ( iSectionArray[i]->GetVertex(1) != vertexn ) )
    UE_LOG(LogTemp,Warning,TEXT("Might be an issue"));
*/
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

    for( uint32 i = 0; i < explorationPairs.size(); i++ )
    {
        if( explorationPairs[i].departSection )
        {
            if( explorationPairs[i].departSection->IsLinked() == true )
            {
                FOdysseyVectorVertex* partnerVertex = explorationPairs[i].intersectionVertex->GetPartner();

                if( explorationPairs[i].departSection->IsBlocked( partnerVertex ) == false )
                {
                    std::vector<FOdysseyVectorVertex*> vertexArray;
                    std::vector<FOdysseyVectorSection*> sectionArray;

                    uint32 ret = FindPath( explorationPairs[i].returnSection
                                         , partnerVertex // is on departSection
                                         , explorationPairs[i].departSection
                                         , vertexArray
                                         , sectionArray
                                         , 1.0f
                                         , 0 );
                }
            }
        }
    }

    return 0;
}

uint32
FOdysseyVectorGroupPaint::MarchGapSegment( FOdysseyVectorSegmentCubic* iGapSegment )
{
    FOdysseyVectorVertex* vertex0 = iGapSegment->GetVertex(0);
    FOdysseyVectorVertex* vertex1 = iGapSegment->GetVertex(1);

    if( ( vertex0->GetClass() == FOdysseyVectorVertex::StaticClass() )
     && ( vertex1->GetClass() == FOdysseyVectorVertex::StaticClass() ) )
    {
        FOdysseyVectorSection* returnSection = vertex0->GetSection( iGapSegment );
        FOdysseyVectorSection* departSection = vertex0->GetOtherSection( returnSection, false );

        if( departSection ) // departSection can be nullptr after graph simplification
        {                   //  when obtained from FOdysseyVectorVertex::GetSection() as it is removed from topology
            if( departSection->IsBlocked( vertex0 ) == false )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                uint32 ret = FindPath( returnSection
                                     , vertex0 // is on departSection
                                     , departSection
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

    // explore the graph from intersections
    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        MarchIntersection( mIntersectionArray[i] );
    }

    // explore the graph from gaps
    for(int i = 0; i < mGapSegmentBuffer.size(); i++)
    {
        MarchGapSegment( &mGapSegmentBuffer[i] );
    }

    CheckLoops();

    OrderCycles();

    MergeCycles();

    Colorize();
}

static FOdysseyVectorVertex*
CreateNearIntersection( FOdysseyVectorVertex *iVertex
                      , std::vector<FOdysseyVectorIntersection*>& iIntersectionArray )
{
    FOdysseyVectorSegment *nearestSegment = iVertex->GetNearestSegment();
    FOdysseyVectorVertex* nearestVertex = nullptr;

    if( nearestSegment )
    {
        double nearestSegmentT = iVertex->GetNearestSegmentT();

        if( ( nearestSegmentT == 0.0f ) || ( nearestSegmentT == 1.0f ) )
        {
            nearestVertex = iVertex->GetNearestSegment()->GetVertex( (int) nearestSegmentT );

            iVertex->SetNearestVertex( nearestVertex );
        }
        else
        {
            ::ULIS::FVec2D nearestVertexAt = nearestSegment->GetPointAt( nearestSegmentT );
            FOdysseyVectorVertexIntersection* intersectionVertex[2] = { new FOdysseyVectorVertexIntersection( nearestSegment->GetPath(), false, nearestVertexAt.x, nearestVertexAt.y, nearestSegmentT )
                                                                        // technically this one should belong to some dummy path
                                                                      , new FOdysseyVectorVertexIntersection( nearestSegment->GetPath(), false, nearestVertexAt.x, nearestVertexAt.y, 0.0f ) };

            iIntersectionArray.push_back( new FOdysseyVectorIntersection( intersectionVertex[0], intersectionVertex[1] ) );

            nearestSegment->AddIntersection( intersectionVertex[0] );

            nearestVertex = intersectionVertex[1];

            iVertex->SetNearestVertex( nearestVertex );
        }

        //UE_LOG(LogTemp,Warning,TEXT("GetSectionCount: %d %d"),intersectionVertex[0]->GetSectionCount(),intersectionVertex[1]->GetSectionCount());
    }

    return nearestVertex;
}

double
FOdysseyVectorGroupPaint::GetGapTolerance()
{
    return mGroupPaintParam.Tolerance;
}

void
FOdysseyVectorGroupPaint::SetGapTolerance( double iGapTolerance )
{
    mGroupPaintParam.Tolerance = iGapTolerance;

    Invalidate();
}

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    std::list<FOdysseyVectorObject*> pathList = mChildrenList; // copy
    uint32 totalGapSegmentCount = 0;
    uint32 totalSectionCount = 0;
    // act as boolean without the need to reinitialize its value
    static uint32 paintingCode;

    mPaintingCode = ++paintingCode;

    Clear();

    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorPath *path = static_cast<FOdysseyVectorPath*>(*oit);
        std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
        std::list<FOdysseyVectorVertex*>& vertexList = path->GetVertexList();
        uint32 intersectionCount = 0;
        uint32 gapSegmentCount = 0;

        for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
        {
            FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*sit);

            for( std::list<FOdysseyVectorObject*>::iterator pit = pathList.begin(); pit != pathList.end(); ++pit )
            {
                FOdysseyVectorPath *intersectPath = static_cast<FOdysseyVectorPath*>(*pit);
                std::list<FOdysseyVectorSegment*>& intersectPathSegmentList = intersectPath->GetSegmentList();

                intersectionCount += IntersectSegment ( segment, intersectPathSegmentList, mIntersectionArray );
            }

            segment->SetPaintingCode( mPaintingCode ); // set as treated. It will be excluded from later intersection tests.

            if( segment->GetIntersectionVertexCount() )
            {
                segment->GetPath()->SetPaintingCode( mPaintingCode );
            }
        }

        // create near-intersections
        for( std::list<FOdysseyVectorVertex*>::iterator vit = vertexList.begin(); vit != vertexList.end(); ++vit )
        {
            FOdysseyVectorVertex *vertex = static_cast<FOdysseyVectorVertex*>(*vit);

            if( vertex->GetNearestSegment() )
            {
                FOdysseyVectorVertex* nearestVertex = CreateNearIntersection( vertex, mIntersectionArray );

                vertex->GetNearestSegment()->GetPath()->SetPaintingCode( mPaintingCode );
                vertex->GetPath()->SetPaintingCode( mPaintingCode );
            }
        }

        if( ( path->IsLoop() == true ) || ( gapSegmentCount ) )
        {
            // acts as a boolean flag
            path->SetPaintingCode( mPaintingCode );
        }

        pathList.pop_front(); // we don't need the path anymore. By and by the list will empty by itself.
    }

    // we now need another loop to count and to reserve the memory in one block to create the sections.
    // we do that so that we can alloc the buffers at once instead of allocating a lot of new sections/segments.
    // This is needed especially with gaps because segments get all their intersections at the end of the whole intersecting process.
    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorPath *path = static_cast<FOdysseyVectorPath*>(*oit);

        if( path->GetPaintingCode() == mPaintingCode )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = (*sit);
                FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
                FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
                FOdysseyVectorVertex* nearestVertex0 = vertex0->GetNearestVertex();
                FOdysseyVectorVertex* nearestVertex1 = vertex1->GetNearestVertex();

                totalSectionCount += ( 1 + segment->GetIntersectionVertexCount() );

                if( nearestVertex0 )
                {
                    if( ( ( nearestVertex0->GetNearestVertex() == vertex0 ) && ( vertex0 > nearestVertex0 ) )
                       || ( nearestVertex0->GetNearestVertex() != vertex0 ) ) // Note : intersections don't have nearest vertices.
                    {
                        totalGapSegmentCount++;
                        //totalSectionCount++;
                    }
                }

                if( nearestVertex1 )
                {
                    if( ( ( nearestVertex1->GetNearestVertex() == vertex1 ) && ( vertex1 > nearestVertex1 ) )
                       || ( nearestVertex1->GetNearestVertex() != vertex1 ) ) // Note : intersections don't have nearest vertices.
                    {
                        totalGapSegmentCount++;
                        //totalSectionCount++;
                    }
                }
            }
        }
    }

    // reserving whole block is required to avoid memory shifting.
    mGapSegmentBuffer.reserve( totalGapSegmentCount );
    // reserving whole block is required to avoid memory shifting.
    mSectionBuffer.reserve( totalSectionCount + totalGapSegmentCount );

    // create sections for exact intersections on each segment

    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorPath *path = static_cast<FOdysseyVectorPath*>(*oit);

        // only for path that have intersected segments.
        if( path->GetPaintingCode() == mPaintingCode )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            CreatePathSections( path, mSectionBuffer, mGapSegmentBuffer );

            for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = (*sit);

                segment->ClearIntersections();
            }
        }
    }

//UE_LOG(LogTemp, Warning, TEXT("%d %d"), mSectionBuffer.size(), mGapSegmentBuffer.size() );
}

void
FOdysseyVectorGroupPaint::MergeCycles()
{
    for(int i = 0; i < mCycleArray.size(); i++)
    {
        FOdysseyVectorCycle* cycle = mCycleArray[i];
        FOdysseyVectorCycle* parentCycle = cycle->GetParentCycle();

        if( parentCycle )
        {
            parentCycle->Merge( cycle );
        }
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
                           innerCycle->SetParentCycle( cycle );
                       }
                   }
                   else
                   {
                       innerCycle->SetParentCycle( cycle );
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

    do
    {
        keepSimplifying = false;

        for( int i = 0; i < mSectionBuffer.size(); i++ )
        {
            FOdysseyVectorSection *section = &mSectionBuffer[i];

            if( section->IsLinked() == true )
            {
                if( section->GetSegment() )
                {
                    if( ( section->GetVertex(0)->GetSectionCount() == 1 )
                    ||  ( section->GetVertex(1)->GetSectionCount() == 1 ) )
                    {
                        keepSimplifying = true;

                        section->Unlink();
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

    for( int i = 0; i < mCycleArray.size(); i++ )
    {
        FOdysseyVectorCycle *cycle = mCycleArray[i];

        delete cycle;
    }

    mCycleArray.clear();



    mGapSegmentBuffer.clear();



    // clean section topology
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        FOdysseyVectorSection *section = &mSectionBuffer[i];

        if( section->IsLinked() == true )
        {
            section->Unlink();
        }
    }

    mSectionBuffer.clear();



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

                // reset nearest segment
                segment->GetVertex(0)->SetNearestSegment( nullptr, mGroupPaintParam.Tolerance/*DBL_MAX*/, 0.0f );
                segment->GetVertex(1)->SetNearestSegment( nullptr, mGroupPaintParam.Tolerance/*DBL_MAX*/, 0.0f );
            }
        }
    }

    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        delete mIntersectionArray[i];
    }

    mIntersectionArray.clear();
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
