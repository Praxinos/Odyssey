#include "OdysseyVectorSegment.h"

UOdysseyVectorSegment::~UOdysseyVectorSegment()
{
}

UOdysseyVectorSegment::UOdysseyVectorSegment()
    : UOdysseyVectorLink ()
    , mPath ( nullptr )
{

}

void
UOdysseyVectorSegment::Init( UOdysseyVectorPath* iPath
                           , UOdysseyVectorVertex* iVertex0
                           , UOdysseyVectorVertex* iVertex1 )
{
    UOdysseyVectorLink::Init ( iVertex0, iVertex1 );

    AddSection ( new FOdysseyVectorSection ( this, iVertex0, iVertex1 ) );

    mPath = iPath;
}

// static
UOdysseyVectorSegment*
UOdysseyVectorSegment::New( UOdysseyVectorPath* iPath
                          , UOdysseyVectorVertex* iVertex0
                          , UOdysseyVectorVertex* iVertex1 )
{
    UOdysseyVectorSegment* segment = NewObject<UOdysseyVectorSegment>();

    segment->Init ( iPath, iVertex0, iVertex1 );

    return segment;
}

UOdysseyVectorPath*
UOdysseyVectorSegment::GetPath()
{
    return mPath;
}

FOdysseyVectorSection*
UOdysseyVectorSegment::GetSection ( double t )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        UOdysseyVectorVertex* vertex0 = section->GetVertex(0);
        UOdysseyVectorVertex* vertex1 = section->GetVertex(1);
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
UOdysseyVectorSegment::AddSection ( FOdysseyVectorSection* iSection )
{
    iSection->GetVertex(0)->AddSection( iSection );
    iSection->GetVertex(1)->AddSection( iSection );

    mSectionList.push_back( iSection );
}

void
UOdysseyVectorSegment::RemoveSection ( FOdysseyVectorSection* iSection )
{
    iSection->GetVertex(0)->RemoveSection( iSection );
    iSection->GetVertex(1)->RemoveSection( iSection );

    mSectionList.remove( iSection );
}

std::list<FOdysseyVectorSection*>&
UOdysseyVectorSegment::GetSectionList()
{
    return mSectionList;
}

void
UOdysseyVectorSegment::AddIntersection ( UOdysseyVectorVertexIntersection* iIntersectionVertex )
{
    double t = iIntersectionVertex->GetT( *this );
    FOdysseyVectorSection* section = GetSection ( t );
    UOdysseyVectorVertex* vertex[3] = { section->GetVertex(0), iIntersectionVertex, section->GetVertex(1) };
    FOdysseyVectorSection* subSection[2] = { new FOdysseyVectorSection ( this, vertex[0], vertex[1] )
                                           , new FOdysseyVectorSection ( this, vertex[1], vertex[2] ) };

    // this does not release memory so that we can undo that later
    RemoveSection( section );

    mIntersectionVertexList.push_back( iIntersectionVertex );

    AddSection ( subSection[0] );
    AddSection ( subSection[1] );
}

void
UOdysseyVectorSegment::ClearIntersections ( )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        UOdysseyVectorVertex* vertex0 = section->GetVertex(0);
        UOdysseyVectorVertex* vertex1 = section->GetVertex(1);

        vertex0->RemoveSection( section );
        vertex1->RemoveSection( section );
    }

    // We do not release memory so that we can undo that later
    mSectionList.clear();

    // We do not release memory so that we can undo that later
    mIntersectionVertexList.clear();

    // Add default section
    AddSection ( new FOdysseyVectorSection ( this, Cast<UOdysseyVectorVertex>(mPoint[0]), Cast<UOdysseyVectorVertex>(mPoint[1]) ) );
}

void
UOdysseyVectorSegment::Invalidate()
{
    for( std::list<UOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = static_cast<UOdysseyVectorVertexIntersection*>(*it);
    }

    mPath->InvalidateSegment( this );
}

void
UOdysseyVectorSegment::DrawStructure( ::ULIS::FRectD &iRoi )
{

}

std::list<UOdysseyVectorVertexIntersection*>&
UOdysseyVectorSegment::GetIntersectionVertexList()
{
    return mIntersectionVertexList;
}

bool
UOdysseyVectorSegment::HasIntersectionVertex( UOdysseyVectorVertexIntersection& mIntersectionVertex )
{
    for( std::list<UOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it )
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = static_cast<UOdysseyVectorVertexIntersection*>(*it);

        if ( intersectionVertex == &mIntersectionVertex ) 
        {
            return true;
        }
    }

    return false;
}

UOdysseyVectorVertex*
UOdysseyVectorSegment::GetNextVertex( double iT )
{
    UOdysseyVectorVertex* closestVertex = nullptr;
    double closestT = 1.0f;

    // Search intersection point between this intersection point and the next segment point
    for(std::list<UOdysseyVectorVertexIntersection*>::iterator it = mIntersectionVertexList.begin(); it != mIntersectionVertexList.end(); ++it)
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = static_cast<UOdysseyVectorVertexIntersection*>(*it);
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

    return ( closestVertex ) ? closestVertex : Cast<UOdysseyVectorVertex>(mPoint[1]);
}

UOdysseyVectorVertex*
UOdysseyVectorSegment::GetPreviousVertex( double iT )
{
    UOdysseyVectorVertex* closestVertex = nullptr;
    double closestT = 0.0f;

    // Search intersection point between this intersection point and the next segment point
    for(std::list<UOdysseyVectorVertexIntersection*>::iterator iter = mIntersectionVertexList.begin(); iter != mIntersectionVertexList.end(); ++iter)
    {
        UOdysseyVectorVertexIntersection* intersectionVertex = static_cast<UOdysseyVectorVertexIntersection*>(*iter);
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

    return ( closestVertex ) ? closestVertex : Cast<UOdysseyVectorVertex>(mPoint[0]);
}

UOdysseyVectorSegment*
UOdysseyVectorSegment::GetNextSegment( )
{
    std::list<UOdysseyVectorSegment*>& segmentList = Cast<UOdysseyVectorVertex>(mPoint[1])->GetSegmentList();

    for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( segment != this ) 
        {
            return segment;
        }
    }

    return nullptr;
}

UOdysseyVectorSegment* UOdysseyVectorSegment::GetPreviousSegment( )
{
    std::list<UOdysseyVectorSegment*>& segmentList = Cast<UOdysseyVectorVertex>(mPoint[0])->GetSegmentList();

    for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( segment != this ) 
        {
            return segment;
        }
    }

    return nullptr;
}

void
UOdysseyVectorSegment::Draw( ::ULIS::FRectD &iRoi )
{

}
