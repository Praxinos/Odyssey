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

FOdysseyVectorGroupPaint::FOdysseyVectorGroupPaint( const FString& iName )
    : FOdysseyVectorGroup( iName )
{
    SetName( iName );

    mDependsOnChildren = true;

    mIntersectionArray.reserve( 60 );

    mGroupPaintParam.Painted = true;
    mGroupPaintParam.Monochrome = false;
    mGroupPaintParam.MonochromeColor = FColor( 128, 128, 128, 255 );
    mGroupPaintParam.GapTolerance = 12.0f;
    mGroupPaintParam.Realtime = false;
    mGroupPaintParam.Wireframe = false;
    mGroupPaintParam.WireframeColor = FColor( 255, 255, 255, 255 );
 
    mBackgroundBucket.SetSolidColor( 128, 128, 128, 0 );

    //mGapSegmentBuffer.reserve( 200 );
    //mSectionBuffer.reserve( 200 );
}

void
FOdysseyVectorGroupPaint::UnselectAllBuckets()
{
    mSelectedBucketList.remove_if( []( FOdysseyVectorBucket* iSelectedBucket )
                                   {
                                       iSelectedBucket->SetSelected( false );

                                       return true;
                                   } );
}

void
FOdysseyVectorGroupPaint::UnselectBucket( FOdysseyVectorBucket* iSelectedBucket )
{
    mSelectedBucketList.remove( iSelectedBucket );

    iSelectedBucket->SetSelected( false );
}

void
FOdysseyVectorGroupPaint::SelectBucket( FOdysseyVectorBucket* iSelectedBucket )
{
    if( iSelectedBucket->IsSelected() == false )
    {
        mSelectedBucketList.push_back( iSelectedBucket );

        iSelectedBucket->SetSelected( true );
    }
}

std::list<FOdysseyVectorBucket*>&
FOdysseyVectorGroupPaint::GetSelectedBucketList()
{
    return mSelectedBucketList;
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

std::list<FOdysseyVectorCycle*>&
FOdysseyVectorGroupPaint::GetCycleList()
{
    return mCycleList;
}

void
FOdysseyVectorGroupPaint::PropagateBuckets()
{
    bool doPropagate = true;

    while( doPropagate )
    {
        std::list<FOdysseyVectorCycle*>::iterator it;

        doPropagate = false;

        for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
        {
            FOdysseyVectorCycle *cycle = (*it);

            if( ( cycle->GetBucket() == nullptr ) && ( cycle->GetPropagatedBucket() == nullptr ) )
            {
                if( cycle->PropagateBucket() == true )
                {
                    doPropagate = true;
                }
            }
        }
    }
}

void
FOdysseyVectorGroupPaint::Colorize()
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    std::list<FOdysseyVectorCycle*>::iterator it;

    blctx->save();
    blctx->setMatrix( mWorldMatrix );

    // reset color for all cycles first
    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);

        cycle->SetBucket( nullptr );
    }

    for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
    {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);

        ApplyBucket( bucket );
    }

    PropagateBuckets();

    blctx->restore();
}

void
FOdysseyVectorGroupPaint::ApplyBucket( FOdysseyVectorBucket* iBucket )
{
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);

        if( cycle->HitTest( iBucket->GetCoords().x, iBucket->GetCoords().y ) )
        {
            cycle->SetBucket( iBucket );

            // a single bucket per loop;
            return;
        }
    }

    /*Invalidate();*/
}

// TODO: Rename this method. this is not a callback anymore
void
FOdysseyVectorGroupPaint::OnChildTransform( FOdysseyVectorObject* iChild )
{
    if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

        path->SwitchSpace( *this );
        path->ResetTransform();
        path->UpdateMatrix( false );// pass false to prevent loop
        path->InvalidateAllSegments();
        path->Update(0);
    }
}

void
FOdysseyVectorGroupPaint::DrawChildren( uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>::iterator it; 

    for( it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath *childPath = static_cast<FOdysseyVectorPath*>(child);

            if( mGroupPaintParam.Wireframe == false )
            {
                childPath->Draw( iFlags );
            }
            else
            {
                childPath->DrawStructure( mGroupPaintParam.WireframeColor, 1.0f, true );
            }
        }
        else
        {
            child->Draw( iFlags );
        }
    }
}

void
FOdysseyVectorGroupPaint::Draw( uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();

    blctx->save();
    blctx->transform( mLocalMatrix );

    blctx->setCompOp( BL_COMP_OP_SRC_OVER );

    DrawShape( iFlags );
    DrawChildren( iFlags );

    blctx->restore();
}

void
FOdysseyVectorGroupPaint::TransferChild( FOdysseyVectorObject* iFosterChild, FOdysseyVectorObject* iInsertAfter )
{
    OnChildTransform( iFosterChild );

    FOdysseyVectorGroup::TransferChild( iFosterChild, iInsertAfter );
}

void
FOdysseyVectorGroupPaint::UpdateShape( uint32 iUpdateFlags )
{
    std::list<FOdysseyVectorPath*>::iterator it;
   BLMatrix2D identityMatrix = BLMatrix2D( BLMatrix2D::makeIdentity() );

    if( mGroupPaintParam.Painted )
    {
        if( ( mGroupPaintParam.Realtime == true  )
       || ( ( mGroupPaintParam.Realtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATEPAINTGROUPS ) ) )
        {
            for( it = mPathList.begin(); it != mPathList.end(); ++it )
            {
                FOdysseyVectorPath* path = *it;

                if( path->GetLocalMatrix() != identityMatrix )
                {
                    OnChildTransform( path );
                }
            }
        }
    }

    FOdysseyVectorGroup::UpdateShape( iUpdateFlags ); // updates BBox

    if( mGroupPaintParam.Painted )
    {
        if( ( mGroupPaintParam.Realtime == true  )
       || ( ( mGroupPaintParam.Realtime == false ) && ( iUpdateFlags & FOdysseyVectorObject::UPDATEPAINTGROUPS ) ) )
        {
            if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_SHAPE )
            {
                Clear();
                FindCycles();

                if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
                {
                    mInvalidationFlags &= (~INVALIDATE_SHAPE);
                }
            }

            if( mInvalidationFlags & FOdysseyVectorObject::INVALIDATE_CHILD )
            {
                Clear();
                FindCycles();

                if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
                {
                    mInvalidationFlags &= (~INVALIDATE_CHILD);
                }
            }

            Colorize();

            if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
            {
                mInvalidationFlags &= (~INVALIDATE_COLOR);
            }
        }
    }
    else
    {
        Clear();
    }
}

/*void
FOdysseyVectorGroupPaint::InvalidateColoring()
{

}*/

void
FOdysseyVectorGroupPaint::AddBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.push_back( iBucket );

    //iBucket->SetParent( this );
    Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorGroupPaint::RemoveBucket( FOdysseyVectorBucket* iBucket )
{
    mBucketList.remove( iBucket );

    if( iBucket->IsSelected() )
    {
        UnselectBucket( iBucket );
    }

    Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorGroupPaint::DrawShape( uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);

        cycle->Draw( iFlags, mGroupPaintParam.Monochrome, mGroupPaintParam.MonochromeColor );
    }

    if( mGroupPaintParam.Wireframe )
    {
        blctx->save();
        blctx->resetMatrix();
        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF, 0x00, 0x00, 0xFF ) );

        for( int i = 0; i < mGapSegmentBuffer.size(); i++ )
        {
            mGapSegmentBuffer[i].DrawStructure( this, true );
        }

        blctx->restore();
    }
}

bool
FOdysseyVectorGroupPaint::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );
        std::list<FOdysseyVectorCycle*>::iterator it;

        for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
        {
            FOdysseyVectorCycle *cycle = (*it);

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
            // Warning: setting a parent path here leads to bugs, due to path update of a segment not really belonging to it.
            iGapSegmentBuffer.emplace_back( nullptr, nearestVertex, iVertex );
            //iGapSegmentBuffer[gapCount].Link(); // not necessary. saves us some cpu cycles

            iSectionBuffer.emplace_back( &iGapSegmentBuffer[gapCount], nearestVertex, iVertex );
            iSectionBuffer[sectionCount].Link();

            ::ULIS::FVec2D delta = iVertex->GetCoords() - nearestVertex->GetCoords();
            double length = delta.Distance() * 0.33f;
/*
            if( ( iVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
             && ( iVertex->GetSectionCount() == 2 ) )
            {
                ::ULIS::FVec2D vertexVector = iVertex->GetFirstSegment()->GetVectorFromVertex( iVertex, true );

                iGapSegmentBuffer[gapCount].GetHandle(1)->Set( iVertex->GetCoords() - ( vertexVector * length ) );
            }

            if( ( nearestVertex->GetClass() != FOdysseyVectorVertexIntersection::StaticClass() )
             && ( nearestVertex->GetSectionCount() == 2 ) )
            {
                ::ULIS::FVec2D vertexVector = nearestVertex->GetFirstSegment()->GetVectorFromVertex( nearestVertex, true );

                iGapSegmentBuffer[gapCount].GetHandle(0)->Set( nearestVertex->GetCoords() - ( vertexVector * length ) );
            }
*/
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
                                                        , mGroupPaintParam.GapTolerance
                                                        , iIntersectionArray );
            }
        }
    }

    return intersectionCount;
}

static void
PrintVertex( FOdysseyVectorVertex* iVertex )
{
    BLPoint pt = iVertex->GetPath()->GetWorldMatrix().mapPoint( iVertex->GetCoords().x, iVertex->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Vertex: [x:%f y:%f]"), pt.x, pt.y);
}

static void
PrintSection( FOdysseyVectorSection* iSection)
{
    FOdysseyVectorPath* path = iSection->GetVertex(0)->GetPath();
    FOdysseyVectorSegment* segment = iSection->GetSegment();
    BLPoint pt0 = path->GetWorldMatrix().mapPoint( iSection->GetVertex(0)->GetCoords().x, iSection->GetVertex(0)->GetCoords().y );
    BLPoint pt1 = path->GetWorldMatrix().mapPoint( iSection->GetVertex(1)->GetCoords().x, iSection->GetVertex(1)->GetCoords().y );
    BLPoint segpt0 = path->GetWorldMatrix().mapPoint( segment->GetVertex(0)->GetCoords().x, segment->GetVertex(0)->GetCoords().y );
    BLPoint segpt1 = path->GetWorldMatrix().mapPoint( segment->GetVertex(1)->GetCoords().x, segment->GetVertex(1)->GetCoords().y );

    UE_LOG(LogTemp,Warning,TEXT("Section: [x:%f y:%f] -- [x:%f y:%f]/segment[x:%f y:%f] -- [x:%f y:%f] - flags : %d"), pt0.x, pt0.y, pt1.x, pt1.y, segpt0.x, segpt0.y, segpt1.x, segpt1.y, iSection->GetFlags() );
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
    //::ULIS::FVec2D sectionVector = -iSection->GetVectorFromVertex( nextVertex, false, false );
    bool hasPrimary = false;
    bool hasSecondary = false;
    bool hasTertiary = false;
    uint32 ret = FOdysseyVectorGroupPaint::NOCYCLE;
    static int i;
    bool isLoop = false;

    iSectionArray.push_back( iSection );
    iVertexArray.push_back( iVertex );
    iSection->Block( iVertex );

//    if( i == 15)
//    {
//            UE_LOG(LogTemp,Warning,TEXT("let's break, nextVertex: %d"), nextVertex );
 //   }

//UE_LOG(LogTemp,Warning,TEXT("%d"), i++ );


    //UE_LOG(LogTemp, Warning, TEXT("exploring section:") );
    //PrintSection( iSection );
    // loop checking if initiator is of type 
    if( iVertexArray[0]->GetClass() == FOdysseyVectorVertexIntersection::StaticClass() )
    {
        FOdysseyVectorVertexIntersection* initiatorVertex = static_cast<FOdysseyVectorVertexIntersection*>(iVertexArray[0]);

        isLoop = initiatorVertex->GetIntersection()->HasVertex( static_cast<FOdysseyVectorVertexIntersection*>(nextVertex) );
    }

    if( iVertexArray[0]->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        isLoop = ( iVertexArray[0] == nextVertex );
    }

    //PrintSection(iSection);
    //UE_LOG(LogTemp,Warning,TEXT("IsLoop: %d %d"), isLoop, iVertexArray.size() );
    //PrintVertex(nextVertex);

    if( ( isLoop == true )// cycle detected
    && ( ( ( iReturnSection->IsLinked() == true ) && ( iReturnSection == iSection ) ) // 1 return path accepted
        || ( iReturnSection->IsLinked() == false ) ) ) // any return path accepted
    {
        //UE_LOG(LogTemp,Warning,TEXT("cycle detected") );

        //PrintCycle( iVertexArray, iSectionArray );

        if( GetNormalVector( iVertexArray, iSectionArray ) > 0.0f )
        {
            //UE_LOG(LogTemp,Warning,TEXT("cycle accepted") );

            mCycleList.push_back( new FOdysseyVectorCycle( this, iVertexArray, iSectionArray ) );
        }

        ret = FOdysseyVectorGroupPaint::HASCYCLE;
    }
    else
    {
        if( nextVertex->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorSection* primaryNextSection = /*nextVertex->GetOtherSection( iSection, false )*/nextVertex->GetCycleNextSection( iSection, 1.0f );

            if( primaryNextSection )
            {
         //UE_LOG(LogTemp,Warning,TEXT("primary flags: %d %d"), primaryNextSection->GetFlags(), nextVertex->GetSectionCount() );
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
            FOdysseyVectorSection* primaryNextSection = nextIntersectionVertex->GetCycleNextSection( iSection, 1.0f );
            FOdysseyVectorSection* secondaryNextSection = nextIntersectionVertex->GetOtherSection( iSection, true );
            FOdysseyVectorSection* tertiaryNextSection = nextIntersectionVertex->GetCycleNextSection( iSection, -1.0f );

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

        ::ULIS::FVec2D& viCoords = vertexi->GetCoords();
        ::ULIS::FVec2D& vnCoords = vertexn->GetCoords();
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
            // however at end points, we need the same coordinates for each section. Relying on T value does not guarantee that
            // due to imprecision and would fake the calculation. So, we use the value stored in viCoords and vnCoords.
            ::ULIS::FVec2D v0Coords = ( j == 0          ) ? viCoords : segment->GetPointAt( t0 );
            ::ULIS::FVec2D v1Coords = ( j == subdiv - 1 ) ? vnCoords : segment->GetPointAt( t1 );
/*
            BLMatrix2D& worldMatrix = segment->GetPath()->GetWorldMatrix();
            BLPoint w0coords = worldMatrix.mapPoint( v0Coords.x, v0Coords.y );
            BLPoint w1coords = worldMatrix.mapPoint( v1Coords.x, v1Coords.y );

UE_LOG(LogTemp,Warning,TEXT("v0coords:t:%f:c:%f %f v1coords:t:%f:c:%f %f"), t0, w0coords.x, w0coords.y, t1, w1coords.x, w1coords.y);

*/
            z += ( ( v0Coords.x - v1Coords.x ) * ( v0Coords.y + v1Coords.y ) );

            t0 += stepT;
        }

// https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
// Newell's method
        //z += ( ( viCoords.x - vnCoords.x ) * ( viCoords.y + vnCoords.y ) );
    }

    //UE_LOG(LogTemp,Warning,TEXT("Normal Z:%f"), z);

    return z;
}

uint32
FOdysseyVectorGroupPaint::Explore( FExplorationPair* iExplorationPair )
{
    if( iExplorationPair->departSection )
    {
        // UE_LOG(LogTemp, Warning, TEXT("Exploring") ); 
        //PrintSection( iExplorationPair->returnSection );
        //PrintSection( iExplorationPair->departSection );


        if( iExplorationPair->departSection->IsLinked() == true )
        {
            if( iExplorationPair->departSection->IsBlocked( iExplorationPair->departVertex ) == false )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;
                uint32 ret = FindPath( iExplorationPair->returnSection
                                     , iExplorationPair->departVertex // is on departSection
                                     , iExplorationPair->departSection
                                     , vertexArray
                                     , sectionArray
                                     , 1.0f
                                     , 0 );
            }// else UE_LOG(LogTemp, Warning, TEXT("Blocked") ); 
        }// else UE_LOG(LogTemp, Warning, TEXT("Unlinked") ); 
    }

    return 0;
}

void
FOdysseyVectorGroupPaint::FindCycles()
{
    std::vector<FExplorationPair> explorationPairsBuffer;

    explorationPairsBuffer.reserve( 100 );

    BuildGraph();

    // Build exploration pair before simplification
    for( int i = 0; i < mIntersectionArray.size(); i++ )
    {
        mIntersectionArray[i]->BuildExplorationPairs( explorationPairsBuffer );
    }

     //UE_LOG(LogTemp, Warning, TEXT("ExpPairs: %d Sections:%d Gaps:%d"), explorationPairsBuffer.size(), mSectionBuffer.size(), mGapSegmentBuffer.size() ); 

    // Build exploration pair before simplification
    for( int i = 0; i < mGapSegmentBuffer.size(); i++ )
    {
        FOdysseyVectorVertex* vertex0 = mGapSegmentBuffer[i].GetVertex(0);
        FOdysseyVectorVertex* vertex1 = mGapSegmentBuffer[i].GetVertex(1);

        //mGapSegmentBuffer[i].BuildExplorationPairs( explorationPairsBuffer );

        if( vertex0->GetClass() == FOdysseyVectorVertex::StaticClass() )
            vertex0->BuildExplorationPairs( explorationPairsBuffer );

        if( vertex1->GetClass() == FOdysseyVectorVertex::StaticClass() )
            vertex1->BuildExplorationPairs( explorationPairsBuffer );
    }

    SimplifyGraph();

    // explore the graph from intersections
    for( int i = 0; i < explorationPairsBuffer.size(); i++ )
    {
        Explore( &explorationPairsBuffer[i] );
    }

    OrderCycles();

    MergeCycles();

    // section topology must be unlinked now or else if we transfer path to another group, it may not be cleaned
    for( int i = 0; i < mSectionBuffer.size(); i++ )
    {
        FOdysseyVectorSection *section = &mSectionBuffer[i];

        if( section->IsLinked() == true )
        {
            section->Unlink();
        }
    }
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
    return mGroupPaintParam.GapTolerance;
}

void
FOdysseyVectorGroupPaint::SetGapTolerance( double iGapTolerance )
{
    mGroupPaintParam.GapTolerance = iGapTolerance;

    Invalidate();
}

bool
FOdysseyVectorGroupPaint::IsMonochrome()
{
    return mGroupPaintParam.Monochrome;
}

void
FOdysseyVectorGroupPaint::SetMonochrome( bool iIsMonochrome )
{
    mGroupPaintParam.Monochrome = iIsMonochrome;
}

FColor&
FOdysseyVectorGroupPaint::GetMonochromeColor()
{
    return mGroupPaintParam.MonochromeColor;
}

void
FOdysseyVectorGroupPaint::SetMonochromeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mGroupPaintParam.MonochromeColor.R = iR;
    mGroupPaintParam.MonochromeColor.G = iG;
    mGroupPaintParam.MonochromeColor.B = iB;
    mGroupPaintParam.MonochromeColor.A = iA;
}

void
FOdysseyVectorGroupPaint::GetMonochromeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA )
{
    oR = mGroupPaintParam.MonochromeColor.R;
    oG = mGroupPaintParam.MonochromeColor.G;
    oB = mGroupPaintParam.MonochromeColor.B;
    oA = mGroupPaintParam.MonochromeColor.A;
}

bool
FOdysseyVectorGroupPaint::IsWireframe()
{
    return mGroupPaintParam.Wireframe;
}

void
FOdysseyVectorGroupPaint::SetWireframe( bool iIsWireframe )
{
    mGroupPaintParam.Wireframe = iIsWireframe;
}

FColor&
FOdysseyVectorGroupPaint::GetWireframeColor()
{
    return mGroupPaintParam.WireframeColor;
}

void
FOdysseyVectorGroupPaint::SetWireframeColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mGroupPaintParam.WireframeColor.R = iR;
    mGroupPaintParam.WireframeColor.G = iG;
    mGroupPaintParam.WireframeColor.B = iB;
    mGroupPaintParam.WireframeColor.A = iA;
}

void
FOdysseyVectorGroupPaint::GetWireframeColor( uint8 &oR, uint8 &oG, uint8& oB, uint8& oA )
{
    oR = mGroupPaintParam.WireframeColor.R;
    oG = mGroupPaintParam.WireframeColor.G;
    oB = mGroupPaintParam.WireframeColor.B;
    oA = mGroupPaintParam.WireframeColor.A;
}

bool
FOdysseyVectorGroupPaint::IsPainted()
{
    return mGroupPaintParam.Painted;
}

void
FOdysseyVectorGroupPaint::SetPainted( bool iPainted )
{
    mGroupPaintParam.Painted = iPainted;

    Invalidate();
}

void
FOdysseyVectorGroupPaint::AddChild( FOdysseyVectorObject* iChild, FOdysseyVectorObject* iInsertAfter )
{
    FOdysseyVectorObject::AddChild( iChild, iInsertAfter );

    if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

        mPathList.push_back( path );
    }
}

void
FOdysseyVectorGroupPaint::RemoveChild( FOdysseyVectorObject* iChild )
{
    FOdysseyVectorObject::RemoveChild( iChild );

    if( iChild->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iChild);

        mPathList.remove( path );
    }
}

void
FOdysseyVectorGroupPaint::BuildGraph()
{
    std::list<FOdysseyVectorPath*> intersectedPathList = mPathList; // copy
    uint32 totalGapSegmentCount = 0;
    uint32 totalSectionCount = 0;
    // act as boolean without the need to reinitialize its value
    static uint32 paintingCode;

    mPaintingCode = ++paintingCode;

    //Clear();

    for( std::list<FOdysseyVectorPath*>::iterator pit = mPathList.begin(); pit != mPathList.end(); ++pit )
    {
        FOdysseyVectorPath *path = (*pit);
        std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();
        std::list<FOdysseyVectorVertex*>& vertexList = path->GetVertexList();
        uint32 intersectionCount = 0;

        for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
        {
            FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*sit);

            for( std::list<FOdysseyVectorPath*>::iterator iit = intersectedPathList.begin(); iit != intersectedPathList.end(); ++iit )
            {
                FOdysseyVectorPath *intersectedPath = (*iit);
                std::list<FOdysseyVectorSegment*>& intersectPathSegmentList = intersectedPath->GetSegmentList();

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

        if( path->IsLoop() == true )
        {
            // acts as a boolean flag
            path->SetPaintingCode( mPaintingCode );
        }

        intersectedPathList.pop_front(); // we don't need the path anymore. By and by the list will empty by itself.
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
//UE_LOG(LogTemp, Warning, TEXT("path:%d"), path->GetIntersectionCount() );
            CreatePathSections( path, mSectionBuffer, mGapSegmentBuffer );

            // create a cycle right now for untouched looped-paths
            if( ( path->IsLoop() == true ) && ( path->HasIntersections() == false ) )
            {
                std::vector<FOdysseyVectorVertex*> vertexArray;
                std::vector<FOdysseyVectorSection*> sectionArray;

                path->ToVertexAndSectionArray( vertexArray, sectionArray );

                if( vertexArray.size() )
                {
                    mCycleList.push_back( new FOdysseyVectorCycle( this, vertexArray, sectionArray ) );
                }
            }

            // Do some clearing here, taking advantage of that loop to save some CPU cycles.
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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);
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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);
        std::list<FOdysseyVectorCycle*>::iterator cit;

        for( cit = mCycleList.begin(); cit != mCycleList.end(); ++cit )
        {
           FOdysseyVectorCycle* innerCycle = (*cit);

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
    std::list<FOdysseyVectorCycle*>::iterator it;

    for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
    {
        FOdysseyVectorCycle *cycle = (*it);

        delete cycle;
    }

    mCycleList.clear();

    // Clear gap segments. Not necessary to unlink them, as these are not linked. Only section are linked.
    mGapSegmentBuffer.clear();


    // clean section topology (unlinking has been moved after the cycle detection).

    mSectionBuffer.clear();



    for( std::list<FOdysseyVectorObject*>::iterator oit = mChildrenList.begin(); oit != mChildrenList.end(); ++oit )
    {
        FOdysseyVectorObject *child = (*oit);

        if( child->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            std::list<FOdysseyVectorSegment*>& pathSegmentList = path->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator sit = pathSegmentList.begin(); sit != pathSegmentList.end(); ++sit )
            {
                FOdysseyVectorSegment *segment = static_cast<FOdysseyVectorSegment*>(*sit);

                // reset nearest segment
                segment->GetVertex(0)->SetNearestSegment( nullptr, mGroupPaintParam.GapTolerance/*DBL_MAX*/, 0.0f );
                segment->GetVertex(1)->SetNearestSegment( nullptr, mGroupPaintParam.GapTolerance/*DBL_MAX*/, 0.0f );
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
FOdysseyVectorGroupPaint::CopyBuckets( FOdysseyVectorGroupPaint* iDestination, bool iSwitchSpace )
{
     BLMatrix2D conversionMatrix;

     if( iSwitchSpace )
     {
         conversionMatrix = iDestination->GetInverseWorldMatrix();
         conversionMatrix.transform( mWorldMatrix );
     }
     else
     {
         conversionMatrix.reset();
     }

     for( std::list<FOdysseyVectorBucket*>::iterator lit = mBucketList.begin(); lit != mBucketList.end(); ++lit )
     {
        FOdysseyVectorBucket *bucket = static_cast<FOdysseyVectorBucket*>(*lit);
        ::ULIS::FVec2D bucketCoords = bucket->GetCoords();
        BLPoint destinationBucketPosition = conversionMatrix.mapPoint( bucketCoords.x, bucketCoords.y );
        FOdysseyVectorBucket *bucketCopy = new FOdysseyVectorBucket( iDestination, 0.0f, 0.0f, bucket->IsPropagated() );

        bucket->Copy( bucketCopy );

        bucketCopy->Set( destinationBucketPosition.x, destinationBucketPosition.y );

        iDestination->AddBucket( bucketCopy );
     }   
}

FOdysseyVectorObject*
FOdysseyVectorGroupPaint::CopyShape()
{
    FOdysseyVectorGroupPaint* groupPaintCopy = new FOdysseyVectorGroupPaint( "Paint Group Copy" );

    groupPaintCopy->mGroupPaintParam = mGroupPaintParam;

    CopyBuckets( groupPaintCopy, false );

    return groupPaintCopy;
}

void
FOdysseyVectorGroupPaint::GetSelectedPoints( std::vector<FOdysseyVectorPoint*>& oPointArray
                                           , ePointSelectionFlags iPointSelectionFlags )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        if( child->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);

            path->GetSelectedPoints( oPointArray, iPointSelectionFlags );
        }
    }

    if( iPointSelectionFlags & ePointSelectionFlags::Bucket )
    {
        for( std::list<FOdysseyVectorBucket*>::iterator it = mSelectedBucketList.begin(); it != mSelectedBucketList.end(); ++it )
        {
            FOdysseyVectorBucket *bucket = (*it);

            oPointArray.push_back( bucket );
        }
    }
}

bool
FOdysseyVectorGroupPaint::GetBBoxFromSelectedVertices( ::ULIS::FRectD& oBBox, bool iWorld )
{
    std::list<FOdysseyVectorObject*>::iterator it;
    bool inited = false;

    for( it = mChildrenList.begin(); it != mChildrenList.end(); ++it )
    {
        FOdysseyVectorObject* child = *it;

        if( child->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(child);
            ::ULIS::FRectD childBBox;

            if( path->GetBBoxFromSelectedVertices( childBBox, true ) )
            {
                oBBox = inited ? oBBox | childBBox
                               : childBBox;

                inited = true;
            }
        }
    }

    if( inited )
    {
        if( iWorld == false )
        {
            double xmin = oBBox.x;
            double xmax = oBBox.x + oBBox.w;
            double ymin = oBBox.y;
            double ymax = oBBox.y + oBBox.h;
            BLPoint p[4] = { mInverseWorldMatrix.mapPoint( xmin, ymin )
                           , mInverseWorldMatrix.mapPoint( xmax, ymin )
                           , mInverseWorldMatrix.mapPoint( xmax, ymax )
                           , mInverseWorldMatrix.mapPoint( xmin, ymax ) };

            oBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p[0].x, p[1].x, p[2].x, p[3].x )
                                              , ::ULIS::FMath::Min4( p[0].y, p[1].y, p[2].y, p[3].y )
                                              , ::ULIS::FMath::Max4( p[0].x, p[1].x, p[2].x, p[3].x )
                                              , ::ULIS::FMath::Max4( p[0].y, p[1].y, p[2].y, p[3].y ) );
        }
    }

    return inited;
}

// Pick from mask image
void
FOdysseyVectorGroupPaint::PickBucket( std::vector<FOdysseyVectorBucket*>& oPickedBucketArray )
{
    BLImage* maskImage = GetScene()->GetEngine()->GetBLMask();
    std::list<FOdysseyVectorBucket*>::iterator it;
    BLImageData imageData;

    maskImage->getData( &imageData );

    for( it = mBucketList.begin(); it != mBucketList.end(); ++it )
    {
        FOdysseyVectorBucket* bucket = (*it);
        ::ULIS::FVec2D& localCoords = bucket->GetCoords();
        // convert bucket coordinates to world coordinates. Easier to detect collision inside the picking circle.
        BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
        int32 x = (int32) worldCoords.x;
        int32 y = (int32) worldCoords.y;

        if( ( x >= 0 ) && ( x < imageData.size.w )
         && ( y >= 0 ) && ( y < imageData.size.h ) )
        {
            uint8 *pixel = static_cast<uint8*>( imageData.pixelData );
            uint32 offset = ( y * imageData.size.w ) + x;
            uint8 pixelValue = pixel[offset];

            if( pixelValue == 255 )
            {
                oPickedBucketArray.push_back( bucket );
            }
        }
    }
}

FOdysseyVectorCycle*
FOdysseyVectorGroupPaint::PickCycle( double iWorldX, double iWorldY )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLPoint localCoord = mInverseWorldMatrix.mapPoint( iWorldX, iWorldY );
    std::list<FOdysseyVectorCycle*>::iterator it;

    if( mBBox.HitTest( ::ULIS::FVec2D( localCoord.x, localCoord.y ) ) )
    {
        for( it = mCycleList.begin(); it != mCycleList.end(); ++it )
        {
            FOdysseyVectorCycle *cycle = (*it);

            // TODO: Bounding volume for cycles for faster search
            if( cycle->HitTest( localCoord.x, localCoord.y ) == true )
            {
                return cycle;
            }
        }
    }

    return nullptr;
}
