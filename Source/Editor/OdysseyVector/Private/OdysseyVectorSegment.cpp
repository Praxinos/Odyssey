#include "OdysseyVectorSegment.h"

FOdysseyVectorSegment::~FOdysseyVectorSegment()
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment()
    : FOdysseyVectorLink ()
    , mIsInvalidated( false )
    , mPath ( nullptr )
{

}

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetOtherVertex( FOdysseyVectorVertex* iVertex )
{
    return ( iVertex == GetVertex(0) ) ? GetVertex(1) : GetVertex(0);
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


void
FOdysseyVectorSegment::Init( FOdysseyVectorPath* iPath
                           , FOdysseyVectorVertex* iVertex0
                           , FOdysseyVectorVertex* iVertex1 )
{
    FOdysseyVectorLink::Init ( iVertex0, iVertex1 );

    AddSection ( new FOdysseyVectorSection ( this, iVertex0, iVertex1 ) );

    mPath = iPath;
}

// static

FOdysseyVectorSegment*
FOdysseyVectorSegment::New( FOdysseyVectorPath* iPath
                          , FOdysseyVectorVertex* iVertex0
                          , FOdysseyVectorVertex* iVertex1 )
{
    FOdysseyVectorSegment* segment = new FOdysseyVectorSegment();

    segment->Init ( iPath, iVertex0, iVertex1 );

    return segment;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorAtStart( bool iNormalize )
{
    ::ULIS::FVec2D vec = GetVertex(1)->GetCoords( nullptr ) - GetVertex(0)->GetCoords( nullptr );

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

FOdysseyVectorSection*
FOdysseyVectorSegment::GetSection ( double t )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorVertex* vertex0 = section->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = section->GetVertex(1);
        double fromT = vertex0->GetT( this );
        double   toT = vertex1->GetT( this );

        if ( ( t >= fromT ) && ( t <= toT ) )
        {
            return section;
        }
    }

    return nullptr;
}

void
FOdysseyVectorSegment::AddSection ( FOdysseyVectorSection* iSection )
{
    iSection->GetVertex(0)->AddSection( iSection );
    iSection->GetVertex(1)->AddSection( iSection );

    mSectionList.push_back( iSection );
}

void
FOdysseyVectorSegment::DeleteSection ( FOdysseyVectorSection* iSection )
{
    iSection->GetVertex(0)->RemoveSection( iSection );
    iSection->GetVertex(1)->RemoveSection( iSection );

    mSectionList.remove( iSection );

    delete iSection;
}

std::list<FOdysseyVectorSection*>&
FOdysseyVectorSegment::GetSectionList()
{
    return mSectionList;
}

void
FOdysseyVectorSegment::AddIntersection ( FOdysseyVectorVertexIntersection* iIntersectionVertex )
{
    double t = iIntersectionVertex->GetT( this );
    FOdysseyVectorSection* section = GetSection ( t );
    FOdysseyVectorVertex* vertex0 = section->GetVertex(0);
    FOdysseyVectorVertex* vertex1 = section->GetVertex(1);
    FOdysseyVectorSection* subSection[2] = { new FOdysseyVectorSection ( this, vertex0            , iIntersectionVertex )
                                           , new FOdysseyVectorSection ( this, iIntersectionVertex, vertex1             ) };

    DeleteSection( section );

    mIntersectionVertexList.push_back( iIntersectionVertex );

    AddSection ( subSection[0] );
    AddSection ( subSection[1] );
}

void
FOdysseyVectorSegment::ClearIntersections ( )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorVertex* vertex0 = section->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = section->GetVertex(1);

        vertex0->RemoveSection( section );
        vertex1->RemoveSection( section );

        delete section;
    }

    mSectionList.clear();

    // do not free the intersection vertex here, as they are shared between segments. Deletion would be called twice.
    // Leave it to the paintgroup.
    mIntersectionVertexList.clear();

    // Add default section
    AddSection ( new FOdysseyVectorSection ( this, GetVertex(0), GetVertex(1) ) );
}

bool
FOdysseyVectorSegment::IsInvalidated()
{
    return mIsInvalidated;
}

void
FOdysseyVectorSegment::Invalidate()
{
    mPath->InvalidateSegment( this );

    mIsInvalidated = true;
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

bool
FOdysseyVectorSegment::HasIntersectionVertex( FOdysseyVectorVertexIntersection& mIntersectionVertex )
{
    for( std::list<FOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(*it);

        if ( intersectionVertex == &mIntersectionVertex ) 
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorSegment::Draw( ::ULIS::FRectD &iRoi )
{

}
