#include "OdysseyVectorSegment.h"

FOdysseyVectorSegment::~FOdysseyVectorSegment()
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment( FOdysseyVectorPath* iPath
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1 )
    : FOdysseyVectorLink( iVertex0, iVertex1 )
    , mPath( iPath )
    , mIsInvalidated( false )
    , mIsPaintingReady( false )
    , mPaintingCode( 0 )
    , mLength( 0.0f )
{
}

uint32
FOdysseyVectorSegment::GetFractionCount()
{
    return mFractionCache.size();
}

std::vector<FOdysseyVectorFraction>&
 FOdysseyVectorSegment::GetFractionCache()
{
    return mFractionCache;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetFractionCacheStartPointInParent()
{
    if( mFractionCache.size() )
    {
        uint32 index = 0;

        return ::ULIS::FVec2D( mFractionCache[index].lineVertexInParent[0].x
                             , mFractionCache[index].lineVertexInParent[0].y );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetFractionCacheEndPointInParent()
{
    if( mFractionCache.size() )
    {
        uint32 index = mFractionCache.size() - 1;

        return ::ULIS::FVec2D( mFractionCache[index].lineVertexInParent[1].x
                             , mFractionCache[index].lineVertexInParent[1].y );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FOdysseyVectorSegment::DrawFractionCache( BLContext* iBLContext )
{
    BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();

    iBLContext->setStrokeWidth( 1.0f );

    for ( int i = 0; i < mFractionCache.size(); i++ )
    {
        // the stroke thing is very slow and slows the all thing, we have to find something better
        //iBLContext.strokePolygon( mFractionCache[i].vertex, 4 );
/*
        BLPoint pt[4] = { { mFractionCache[i].polygon.point[0].x, mFractionCache[i].polygon.point[0].y }
                        , { mFractionCache[i].polygon.point[1].x, mFractionCache[i].polygon.point[1].y }
                        , { mFractionCache[i].polygon.point[2].x, mFractionCache[i].polygon.point[2].y }
                        , { mFractionCache[i].polygon.point[3].x, mFractionCache[i].polygon.point[3].y } };
*/
        iBLContext->fillPolygon( mFractionCache[i].polygon.point, 4 );
    }

    // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
    // line stroking is done in world coordinates because we need a 1 pixel width
    iBLContext->save();
    iBLContext->resetMatrix();
    iBLContext->setStrokeWidth( 1.0f );
    for ( int i = 1; i < mFractionCache.size(); i++ )
    {
        iBLContext->strokeLine( worldMatrix.mapPoint( mFractionCache[i].polygon.point[0].x, mFractionCache[i].polygon.point[0].y )
                              , worldMatrix.mapPoint( mFractionCache[i].polygon.point[3].x, mFractionCache[i].polygon.point[3].y ) );
    }
    iBLContext->restore();
}

FOdysseyVectorHandleSegment*
FOdysseyVectorSegment::GetHandle( int iCtrlPointNum )
{
    return nullptr;
}

FOdysseyVectorHandleSegment*
FOdysseyVectorSegment::GetHandle( FOdysseyVectorVertex* iVertex )
{
    return nullptr;
}

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetOtherVertex( FOdysseyVectorVertex* iVertex )
{
    return ( iVertex == GetVertex(0) ) ? GetVertex(1) : GetVertex(0);
}

void
FOdysseyVectorSegment::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FOdysseyVectorSegment::GetID()
{
    return mID;
}

void
FOdysseyVectorSegment::SetPaintingCode( uint32 iPaintingCode )
{
    mPaintingCode = iPaintingCode;
}

uint32
FOdysseyVectorSegment::GetPaintingCode()
{
    return mPaintingCode;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetPointAt( double t )
{
    ::ULIS::FVec2D unimplemented;

    return unimplemented;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetTangentAt( double t, bool iNormalize )
{
    ::ULIS::FVec2D unimplemented;

    return unimplemented;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorAtStart( bool iNormalize )
{
    ::ULIS::FVec2D vec = GetVertex(1)->GetCoords() - GetVertex(0)->GetCoords();

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

void
FOdysseyVectorSegment::Link()
{
    GetVertex(0)->AddSegment(this);
    GetVertex(1)->AddSegment(this);
}

void
FOdysseyVectorSegment::Unlink()
{
    GetVertex(0)->RemoveSegment(this);
    GetVertex(1)->RemoveSegment(this);
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorAtEnd( bool iNormalize )
{
    return - GetVectorAtStart( iNormalize );
}

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetVertex( uint32 iVertexID )
{
    return static_cast<FOdysseyVectorVertex*>(GetPoint( iVertexID ));
}

void
FOdysseyVectorSegment::SetPath( FOdysseyVectorPath* iPath )
{
    mPath = iPath;
}

FOdysseyVectorPath*
FOdysseyVectorSegment::GetPath()
{
    return mPath;
}

// MUST be called only on segments belonging to this path (because of the section)
void
FOdysseyVectorSegment::AddIntersection ( FOdysseyVectorVertexIntersection* iIntersectionVertex )
{
    double t = iIntersectionVertex->GetT( this );
    std::list<FOdysseyVectorVertexIntersection*>::iterator it = std::find_if ( mIntersectionVertexList.begin()
                                                                             , mIntersectionVertexList.end()
                                                                             , [&t,this]( FOdysseyVectorVertexIntersection* iVertex )
                                                                               {

                                                                                   return ( iVertex->GetT( this ) > t );
                                                                               } );

    mIntersectionVertexList.insert( it, iIntersectionVertex );

/*
    FOdysseyVectorSection* section = GetSection ( iIntersectionVertex->GetT( this ) );
    FOdysseyVectorVertex* vertex0 = section->GetVertex(0);
    FOdysseyVectorVertex* vertex1 = section->GetVertex(1);
    FOdysseyVectorSection* subSection[2] = { new FOdysseyVectorSection ( this, vertex0            , iIntersectionVertex )
                                           , new FOdysseyVectorSection ( this, iIntersectionVertex, vertex1             ) };
*/
    //if( t )
    

   // mIntersectionVertexList.push_back( iIntersectionVertex );
}

// MUST be called only on segments belonging to this path (because of the section)
void
FOdysseyVectorSegment::ClearIntersections()
{
    // do not free the intersection vertex here
    // Leave it to the paintgroup.
    mIntersectionVertexList.clear();
}

bool
FOdysseyVectorSegment::IsInvalidated()
{
    return mIsInvalidated;
}

void
FOdysseyVectorSegment::Invalidate()
{
    if( mPath ) // Note: GroupPaint "gap segments" can be orphan
    {
        mPath->InvalidateSegment( this );

        mIsInvalidated = true;

        mIsPaintingReady = false;
    }
}

void
FOdysseyVectorSegment::Update()
{
    mIsInvalidated = false;
}

std::list<FOdysseyVectorVertexIntersection*>&
FOdysseyVectorSegment::GetIntersectionVertexList()
{
    return mIntersectionVertexList;
}

uint32
FOdysseyVectorSegment::GetIntersectionVertexCount()
{
    return mIntersectionVertexList.size();
}

void
FOdysseyVectorSegment::GetIntersectionVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    for( std::list<FOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(*it);

        oVertexArray.push_back( intersectionVertex );
    }
}

bool
FOdysseyVectorSegment::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return false;
}

void
FOdysseyVectorSegment::SetPaintingReady( bool iIsPaintingReady )
{
    mIsPaintingReady = iIsPaintingReady;
}

bool
FOdysseyVectorSegment::IsPaintingReady()
{
    return mIsPaintingReady;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetHandleVector( uint32 iHandleID, bool iNormalize )
{
    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetHandleVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FOdysseyVectorSegment::GetAllVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    oVertexArray.push_back( static_cast<FOdysseyVectorVertex*>(mPoint[0]) );
    GetIntersectionVertices( oVertexArray );
    oVertexArray.push_back( static_cast<FOdysseyVectorVertex*>(mPoint[1]) );
}

void
FOdysseyVectorSegment::Draw( BLContext* iBLContext )
{

}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorFromVertex( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    ::ULIS::FVec2D vector = ::ULIS::FVec2D( 0.0f, 0.0f );

    return vector;
}

double
FOdysseyVectorSegment::GetLength()
{
    return ::ULIS::FVec2D( mPoint[1]->GetCoords() - mPoint[0]->GetCoords() ).Distance();
}

/*
void
FOdysseyVectorSegment::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    GetVertex(0)->BuildExplorationPairs( iExplorationPairsArray );
    GetVertex(1)->BuildExplorationPairs( iExplorationPairsArray );
}
*/
