#include "OdysseyVectorSegment.h"

FOdysseyVectorSegment::~FOdysseyVectorSegment()
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment()
    : mPath( nullptr )
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment( FOdysseyVectorPath* iPath
                                            , FOdysseyVectorVertex* iVertex0
                                            , FOdysseyVectorVertex* iVertex1 )
    : FOdysseyVectorLink ()
    , mIsInvalidated( false )
    , mPath ( nullptr )
    , mPaintingCode ( 0 )
{
    FOdysseyVectorLink::Init ( iVertex0, iVertex1 );

    mPath = iPath;
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
FOdysseyVectorSegment::GetTangentAt( double t )
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

void
FOdysseyVectorSegment::GetAllVertices( std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    oVertexArray.push_back( static_cast<FOdysseyVectorVertex*>(mPoint[0]) );
    GetIntersectionVertices( oVertexArray );
    oVertexArray.push_back( static_cast<FOdysseyVectorVertex*>(mPoint[1]) );
}

void
FOdysseyVectorSegment::Draw( ::ULIS::FRectD &iRoi )
{

}
