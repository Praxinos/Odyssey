#include "OdysseyVectorSegment.h"

FOdysseyVectorSegment::~FOdysseyVectorSegment()
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment()
    : FOdysseyVectorLink ()
    , mPath ( nullptr )
{

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
FOdysseyVectorSegment::Init( UOdysseyVectorPath* iPath
                           , FOdysseyVectorVertex* iVertex0
                           , FOdysseyVectorVertex* iVertex1 )
{
    FOdysseyVectorLink::Init ( iVertex0, iVertex1 );

    //AddSection ( new FOdysseyVectorSection ( this, iVertex0, iVertex1 ) );

    mPath = iPath;
}

// static
FOdysseyVectorSegment*
FOdysseyVectorSegment::New( UOdysseyVectorPath* iPath
                          , FOdysseyVectorVertex* iVertex0
                          , FOdysseyVectorVertex* iVertex1 )
{
    FOdysseyVectorSegment* segment = NewObject<FOdysseyVectorSegment>();

    segment->Init ( iPath, iVertex0, iVertex1 );

    return segment;
}

::ULIS::FVec2D
FOdysseyVectorSegment::GetVectorAtStart( bool iNormalize )
{
    ::ULIS::FVec2D vec = mPoint[1]->GetCoords() - mPoint[0]->GetCoords();

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
    return Cast<FOdysseyVectorVertex>(GetPoint( iVertexID ));
}

void
FOdysseyVectorSegment::SetPath( UOdysseyVectorPath* iPath )
{
    mPath = iPath;
}

UOdysseyVectorPath*
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
        double fromT = vertex0->GetT( *this );
        double   toT = vertex1->GetT( *this );

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
    double t = iIntersectionVertex->GetT( *this );
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

    for( std::list<FOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(*it);

        delete intersectionVertex;
    }

    mIntersectionVertexList.clear();

    // Add default section
    AddSection ( new FOdysseyVectorSection ( this, Cast<FOdysseyVectorVertex>(mPoint[0]), Cast<FOdysseyVectorVertex>(mPoint[1]) ) );
}

void
FOdysseyVectorSegment::Invalidate()
{
    for( std::list<FOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(*it);
    }

    mPath->InvalidateSegment( this );
}

void
FOdysseyVectorSegment::DrawStructure( ::ULIS::FRectD &iRoi )
{

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

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetNextVertex( double iT )
{
    FOdysseyVectorVertex* closestVertex = nullptr;
    double closestT = 1.0f;

    // Search intersection point between this intersection point and the next segment point
    for(std::list<FOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it)
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(*it);
        double vertexT = intersectionVertex->GetT(*this);

        if( vertexT > iT )
        {
            if ( vertexT <= closestT )
            {
                closestVertex = intersectionVertex;

                closestT = vertexT;
            }
        }
    }

    return ( closestVertex ) ? closestVertex : Cast<FOdysseyVectorVertex>(mPoint[1]);
}

FOdysseyVectorVertex*
FOdysseyVectorSegment::GetPreviousVertex( double iT )
{
    FOdysseyVectorVertex* closestVertex = nullptr;
    double closestT = 0.0f;

    // Search intersection point between this intersection point and the next segment point
    for(std::list<FOdysseyVectorVertexIntersection*>::iterator iter = mIntersectionVertexList.begin(); iter != mIntersectionVertexList.end(); ++iter)
    {
        FOdysseyVectorVertexIntersection* intersectionVertex = static_cast<FOdysseyVectorVertexIntersection*>(*iter);
        double vertexT = intersectionVertex->GetT(*this);

        if( vertexT < iT )
        {
            if ( vertexT >= closestT )
            {
                closestVertex = intersectionVertex;

                closestT = vertexT;
            }
        }
    }

    return ( closestVertex ) ? closestVertex : Cast<FOdysseyVectorVertex>(mPoint[0]);
}

FOdysseyVectorSegment*
FOdysseyVectorSegment::GetNextSegment( )
{
    std::list<FOdysseyVectorSegment*>& segmentList = Cast<FOdysseyVectorVertex>(mPoint[1])->GetSegmentList();

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( segment != this ) 
        {
            return segment;
        }
    }

    return nullptr;
}

FOdysseyVectorSegment* FOdysseyVectorSegment::GetPreviousSegment( )
{
    std::list<FOdysseyVectorSegment*>& segmentList = Cast<FOdysseyVectorVertex>(mPoint[0])->GetSegmentList();

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( segment != this ) 
        {
            return segment;
        }
    }

    return nullptr;
}

void
FOdysseyVectorSegment::Draw( ::ULIS::FRectD &iRoi )
{

}
