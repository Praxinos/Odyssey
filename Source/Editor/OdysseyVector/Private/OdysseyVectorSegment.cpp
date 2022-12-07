#include "OdysseyVectorSegment.h"

FOdysseyVectorSegment::~FOdysseyVectorSegment()
{
}

FOdysseyVectorSegment::FOdysseyVectorSegment( FOdysseyVectorPath& iPath
                              , FOdysseyVectorPoint* iPoint0
                              , FOdysseyVectorPoint* iPoint1 )
    : FOdysseyVectorLink ( iPoint0, iPoint1 )
    , mPath ( iPath )
{
    /*AddSection ( new FOdysseyVectorSection ( *this, mPoint[0], mPoint[1] ) );*/
}

FOdysseyVectorPath&
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
        FOdysseyVectorPoint* point0 = section->GetPoint(0);
        FOdysseyVectorPoint* point1 = section->GetPoint(1);
        double fromT = point0->GetT( *this );
        double   toT = point1->GetT( *this );

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
    iSection->GetPoint(0)->AddSection( iSection );
    iSection->GetPoint(1)->AddSection( iSection );

    mSectionList.push_back( iSection );
}

void
FOdysseyVectorSegment::RemoveSection ( FOdysseyVectorSection* iSection )
{
    std::list<FOdysseyVectorLoop*> loopList = iSection->GetLoopList();

    iSection->GetPoint(0)->RemoveSection( iSection );
    iSection->GetPoint(1)->RemoveSection( iSection );

    mSectionList.remove( iSection );

    for( std::list<FOdysseyVectorLoop*>::iterator it = loopList.begin(); it != loopList.end(); ++it )
    {
        FOdysseyVectorLoop* loop = static_cast<FOdysseyVectorLoop*>(*it);

        mPath.RemoveLoop( loop );
    }
}

void
FOdysseyVectorSegment::AddIntersection ( FOdysseyVectorPointIntersection* iIntersectionPoint )
{
    double t = iIntersectionPoint->GetT( *this );
    FOdysseyVectorSection* section = GetSection ( t );
    FOdysseyVectorPoint* point[3] = { section->GetPoint(0), iIntersectionPoint, section->GetPoint(1) };
    FOdysseyVectorSection* subSection[2] = { new FOdysseyVectorSection ( *this, point[0], point[1] )
                                    , new FOdysseyVectorSection ( *this, point[1], point[2] ) };

    // this does not release memory so that we can undo that later
    RemoveSection( section );

    mIntersectionPointList.push_back( iIntersectionPoint );

    AddSection ( subSection[0] );
    AddSection ( subSection[1] );
}

void
FOdysseyVectorSegment::ClearIntersections ( )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorPoint* point0 = section->GetPoint(0);
        FOdysseyVectorPoint* point1 = section->GetPoint(1);

        point0->RemoveSection( section );
        point1->RemoveSection( section );
    }

    // We do not release memory so that we can undo that later
    mSectionList.clear();

    // We do not release memory so that we can undo that later
    mIntersectionPointList.clear();

    // Add default section
    AddSection ( new FOdysseyVectorSection ( *this, mPoint[0], mPoint[1] ) );
}

void
FOdysseyVectorSegment::Invalidate()
{
    for( std::list<FOdysseyVectorPointIntersection*>::iterator it = mIntersectionPointList.begin(); it != mIntersectionPointList.end(); ++it )
    {
        FOdysseyVectorPointIntersection* intersectionPoint = static_cast<FOdysseyVectorPointIntersection*>(*it);

        intersectionPoint->InvalidateLoops();
    }

    mPath.InvalidateSegment( this );
}

void
FOdysseyVectorSegment::DrawStructure( ::ULIS::FRectD &iRoi )
{

}

std::list<FOdysseyVectorPointIntersection*>&
FOdysseyVectorSegment::GetIntersectionPointList()
{
    return mIntersectionPointList;
}

bool
FOdysseyVectorSegment::HasIntersectionPoint( FOdysseyVectorPointIntersection& mIntersectionPoint )
{
    for( std::list<FOdysseyVectorPointIntersection*>::iterator it = mIntersectionPointList.begin(); it != mIntersectionPointList.end(); ++it )
    {
        FOdysseyVectorPointIntersection* intersectionPoint = static_cast<FOdysseyVectorPointIntersection*>(*it);

        if ( intersectionPoint == &mIntersectionPoint ) 
        {
            return true;
        }
    }

    return false;
}

FOdysseyVectorPoint*
FOdysseyVectorSegment::GetNextPoint( double iT )
{
    FOdysseyVectorPoint* closestPoint = nullptr;
    double closestT = 1.0f;

    // Search intersection point between this intersection point and the next segment point
    for(std::list<FOdysseyVectorPointIntersection*>::iterator it = mIntersectionPointList.begin(); it != mIntersectionPointList.end(); ++it)
    {
        FOdysseyVectorPointIntersection* intersectionPoint = static_cast<FOdysseyVectorPointIntersection*>(*it);
        double pointT = intersectionPoint->GetT(*this);

        if( pointT > iT )
        {
            if ( pointT <= closestT )
            {
                closestPoint = intersectionPoint;

                closestT = pointT;
            }
        }
    }

    return ( closestPoint ) ? closestPoint : mPoint[1];
}

FOdysseyVectorPoint*
FOdysseyVectorSegment::GetPreviousPoint( double iT )
{
    FOdysseyVectorPoint* closestPoint = nullptr;
    double closestT = 0.0f;

    // Search intersection point between this intersection point and the next segment point
    for(std::list<FOdysseyVectorPointIntersection*>::iterator iter = mIntersectionPointList.begin(); iter != mIntersectionPointList.end(); ++iter)
    {
        FOdysseyVectorPointIntersection* intersectionPoint = static_cast<FOdysseyVectorPointIntersection*>(*iter);
        double pointT = intersectionPoint->GetT(*this);

        if( pointT < iT )
        {
            if ( pointT >= closestT )
            {
                closestPoint = intersectionPoint;

                closestT = pointT;
            }
        }
    }

    return ( closestPoint ) ? closestPoint : mPoint[0];
}

FOdysseyVectorSegment*
FOdysseyVectorSegment::GetNextSegment( )
{
    std::list<FOdysseyVectorSegment*>& segmentList = mPoint[1]->GetSegmentList();

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
    std::list<FOdysseyVectorSegment*>& segmentList = mPoint[0]->GetSegmentList();

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
