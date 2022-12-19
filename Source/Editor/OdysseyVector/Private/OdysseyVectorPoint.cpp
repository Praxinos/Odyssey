#include "OdysseyVectorPoint.h"

FOdysseyVectorPoint::~FOdysseyVectorPoint()
{
}

FOdysseyVectorPoint::FOdysseyVectorPoint()
{
    Set( 0.0f, 0.0f );
}

FOdysseyVectorPoint::FOdysseyVectorPoint( double iX, double iY, double iRadius )
{
    Set( iX, iY );
    SetRadius( iRadius );
}

FOdysseyVectorPoint::FOdysseyVectorPoint( double iX, double iY )
{
    Set( iX, iY );
}

::ULIS::FVec2D&
FOdysseyVectorPoint::GetCoords()
{
    return mCoords;
}

double 
FOdysseyVectorPoint::GetX()
{
    return mCoords.x;
}

double 
FOdysseyVectorPoint::GetY()
{
    return mCoords.y;
}

void
FOdysseyVectorPoint::InvalidateSegments()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        segment->Invalidate();
    }
}

void 
FOdysseyVectorPoint::SetX( double iX )
{
    mCoords.x  = iX;

    InvalidateSegments();
}

void 
FOdysseyVectorPoint::SetY( double iY )
{
    mCoords.y = iY;

    InvalidateSegments();
}

void 
FOdysseyVectorPoint::Set( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;

    InvalidateSegments();
}

void
FOdysseyVectorPoint::AddSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );
}

void
FOdysseyVectorPoint::RemoveSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );
}

uint32
FOdysseyVectorPoint::GetSegmentCount( )
{
    return mSegmentList.size();
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorPoint::GetSegmentList()
{
    return mSegmentList;
}

uint32
FOdysseyVectorPoint::GetType()
{
    return POINT_TYPE_REGULAR; // default type;
}

double
FOdysseyVectorPoint::GetRadius()
{
    return mRadius;
}

void
FOdysseyVectorPoint::SetRadius( double iRadius )
{
    mRadius = iRadius;
}

FOdysseyVectorSegment*
FOdysseyVectorPoint::GetLastSegment()
{
    return mSegmentList.back();
}

FOdysseyVectorSegment*
FOdysseyVectorPoint::GetFirstSegment()
{
    return mSegmentList.front();
}

static uintptr_t
GenerateSegmentID( FOdysseyVectorSegment* iSegment, FOdysseyVectorPoint* iP0, FOdysseyVectorPoint* iP1 )
{
    uintptr_t xorPoint = reinterpret_cast<uintptr_t>(iP0) ^ reinterpret_cast<uintptr_t>(iP1);

    return xorPoint ^ reinterpret_cast<uintptr_t>(iSegment);
}

static bool seekPoint( std::list<FOdysseyVectorPoint*>& iPointList
                     , FOdysseyVectorPoint* iPoint )
{
    for( std::list<FOdysseyVectorPoint*>::iterator it = iPointList.begin(); it != iPointList.end(); ++it )
    {
        if ( static_cast<FOdysseyVectorPoint*>(*it) == iPoint )
        {
            return true;
        }
    }

    return false;
}

double
FOdysseyVectorPoint::GetT( FOdysseyVectorSegment& iSegment )
{
    return ( iSegment.GetPoint(0) == this ) ? 0.0f : 1.0f;
}

static bool seekSection( std::list<FOdysseyVectorSection*>& iSectionList
                       , FOdysseyVectorSection *iSection )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = iSectionList.begin(); it != iSectionList.end(); ++it )
    {
        if ( static_cast<FOdysseyVectorSection*>(*it) == iSection )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorPoint::AddSection( FOdysseyVectorSection* iSection )
{
    mSectionList.push_back( iSection );
}

void
FOdysseyVectorPoint::RemoveSection( FOdysseyVectorSection* iSection )
{
    mSectionList.remove( iSection );
}

bool
FOdysseyVectorPoint::HasSegment( FOdysseyVectorSegment& iSegment )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( segment == &iSegment )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorPoint::AddLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.push_back( iLoop );
}

void
FOdysseyVectorPoint::RemoveLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.remove( iLoop );
}

void
FOdysseyVectorPoint::InvalidateLoops()
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        loop->Invalidate();
    }
}

bool
FOdysseyVectorPoint::IsClosestSection( FOdysseyVectorSection& iStartSection
                              , FOdysseyVectorSection& iEndSection )
{
    double startSectionP0T   = iStartSection.GetPoint( 0 )->GetT( iStartSection.GetSegment() );
    double startSectionP1T   = iStartSection.GetPoint( 1 )->GetT( iStartSection.GetSegment() );
    double startSectionT     = ( startSectionP0T + startSectionP1T ) * 0.5f;
    ::ULIS::FVec2D startSectionMidAt = iStartSection.GetSegment().GetPointAt( startSectionT );

    double endSectionP0T     = iEndSection.GetPoint( 0 )->GetT( iEndSection.GetSegment() );
    double endSectionP1T     = iEndSection.GetPoint( 1 )->GetT( iEndSection.GetSegment() );
    double endSectionT       = ( endSectionP0T + endSectionP1T ) * 0.5f;
    ::ULIS::FVec2D endSectionMidAt   = iEndSection.GetSegment().GetPointAt( endSectionT );

    ::ULIS::FVec3F originToStartMid = { startSectionMidAt.x - this->GetCoords().x
                                      , startSectionMidAt.y - this->GetCoords().y, 0.0f };
    ::ULIS::FVec3F originToEndMid   = { endSectionMidAt.x - this->GetCoords().x
                                      , endSectionMidAt.y - this->GetCoords().y, 0.0f };

    if ( originToStartMid.DistanceSquared() && originToEndMid.DistanceSquared() )
    {
        originToStartMid.Normalize();
        originToEndMid.Normalize();

        ::ULIS::FVec3F referenceCross = originToStartMid.CrossProduct( originToEndMid );

        for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
        {
            FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

            if ( ( section != &iStartSection ) && ( section != &iEndSection ) )
            {
                double p0T = section->GetPoint(0)->GetT(section->GetSegment());
                double p1T = section->GetPoint(1)->GetT(section->GetSegment());
                double t = ( p0T + p1T ) * 0.5f;
                ::ULIS::FVec2D pointAt = section->GetSegment().GetPointAt( t );
                ::ULIS::FVec3F originToPoint = { pointAt.x - this->GetCoords().x
                                               , pointAt.y - this->GetCoords().y, 0.0f };

               if ( originToPoint.DistanceSquared() )
               {
                    originToPoint.Normalize();

                    ::ULIS::FVec3F sectionCross = originToStartMid.CrossProduct( originToPoint );

                    // same side
                    if ( sectionCross.DotProduct( referenceCross ) > 0.0f )
                    {
                        if ( originToStartMid.DotProduct( originToPoint ) > originToStartMid.DotProduct( originToEndMid ) )
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

::ULIS::FRectD
FOdysseyVectorPoint::GetRectangle()
{
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    bool inited = false;

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);
        ::ULIS::FRectD rect = segment->GetBoundingBox();

        bbox = ( inited == false ) ? rect : bbox | rect;

        inited = true;
    }

    return bbox;
}

::ULIS::FVec2D
FOdysseyVectorPoint::GetPosition( FOdysseyVectorSegment& iSegment )
{
    return mCoords;
}

std::list<FOdysseyVectorSection*>&
FOdysseyVectorPoint::GetSectionList()
{
    return mSectionList;
}

typedef struct _TREENODE
{
    struct _TREENODE* mParent;
    FOdysseyVectorPoint* mPoint;
    FOdysseyVectorSection* mSection;
    _TREENODE( struct _TREENODE* iParent, FOdysseyVectorPoint* iPoint, FOdysseyVectorSection* iSection )
        : mParent ( iParent )
        , mPoint ( iPoint )
        , mSection ( iSection ) {};
} TREENODE;

void TREENODETOLIST( TREENODE* node
                   , std::list<FOdysseyVectorPoint*>& iPointList
                   , std::list<FOdysseyVectorSection*>& iSectionList )
{
    while ( node->mSection )
    {
        iPointList.push_back ( node->mPoint );
        iSectionList.push_back ( node->mSection );

        node = node->mParent;
    }
}

bool TREENODESEEKSECTION( TREENODE* node, FOdysseyVectorSection* section )
{
    do
    {
        if ( section == node->mSection )
        {
            return true;
        }

        node = node->mParent;
    } while ( node );

    return false;
}

void
FOdysseyVectorPoint::March()
{
    static int depth; // inits to zero by standard
    FOdysseyVectorPoint* sourcePoint = this;
    uint32 maxLoopcount = sourcePoint->GetSectionList().size();
    TREENODE *node = new TREENODE ( NULL, this, NULL );
    std::list<TREENODE*> nodeList;

    depth++;

    nodeList.push_back ( node );

    while ( nodeList.size() )
    {
        TREENODE *parentNode = nodeList.back();

        std::list<FOdysseyVectorSection*>& sectionList = parentNode->mPoint->GetSectionList();

        for( std::list<FOdysseyVectorSection*>::iterator it = sectionList.begin(); it != sectionList.end(); ++it )
        {
            FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

            if ( /*section != parentNode->mSection*/ TREENODESEEKSECTION( parentNode, section ) == false )
            {
                FOdysseyVectorPoint* nextPoint = ( parentNode->mPoint == section->GetPoint(0) ) ? section->GetPoint(1) : section->GetPoint(0);
                TREENODE* nextNode = new TREENODE ( parentNode, nextPoint, section );

                if ( nextPoint == this )
                {
                    UOdysseyVectorPath& path = nextNode->mSection->GetSegment().GetPath();
                    std::list<FOdysseyVectorPoint*> loopPointList;
                    std::list<FOdysseyVectorSection*> loopSectionList;
                    uint64 loopID;

                    TREENODETOLIST ( nextNode, loopPointList, loopSectionList );
printf("loop detected\n");
                    loopID = UOdysseyVectorLoop::GenerateID( loopSectionList );

                    if ( path.GetLoopByID( loopID ) == nullptr )
                    {
                        if ( IsClosestSection ( *loopSectionList.front(), *loopSectionList.back() ) == true )
                        {
                            UOdysseyVectorLoop *loop = NewObject<UOdysseyVectorLoop>();

                            loop->Init( &path, loopID, nextNode->mPoint, loopPointList, loopSectionList );

                            path.AddLoop( loop );
                        }
                    }
                    else
                    {
printf("path already exists\n");
                    }

                    delete nextNode;
                }
                else
                {
                    nodeList.push_back( nextNode );
                }
            }
        }

        nodeList.remove( parentNode );
    }

    depth--;
}

#ifdef ORIGINAL_VERSION
void
FOdysseyVectorPoint::March( FOdysseyVectorPointIntersection& iInitiatorPoint
                   , FOdysseyVectorSegment& iCurrentSegment )
{
    static std::list<FOdysseyVectorPoint*> loopPointList;
    static std::list<uintptr_t> iSegmentIDList;

    loopPointList.push_back ( this );

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);
        double t =  static_cast<FOdysseyVectorPointIntersection*>(this)->GetT(*segment);
        FOdysseyVectorPoint* forwardPoint = segment->GetNextPoint(t);
        FOdysseyVectorPoint* backwardPoint = segment->GetPreviousPoint(t);

        if ( forwardPoint )
        {
            if ( seekPoint( loopPointList, forwardPoint ) == false )
            {
                iSegmentIDList.push_back( GenerateSegmentID( segment, this, forwardPoint) );
                forwardPoint->March( iInitiatorPoint, *segment );
                iSegmentIDList.pop_back();
            }
            else
            {
                if ( seekSegment( iSegmentIDList, GenerateSegmentID( segment, this, forwardPoint ) ) == false )
                {
                    uint64 loopID = FOdysseyVectorPathLoop::GenerateID( loopPointList, forwardPoint );

                    if ( segment->GetPath().GetPathLoopByID( loopID ) == nullptr )
                    {
                        FOdysseyVectorPathLoop *loopPath = new FOdysseyVectorPathLoop ( segment->GetPath(), loopID, loopPointList, forwardPoint );

                        segment->GetPath().AddLoop( loopPath );
                    } else printf("loop exists!!\n");
                }
            }
        }

        if( backwardPoint )
        {
            if( seekPoint( loopPointList, backwardPoint ) == false )
            {
                iSegmentIDList.push_back( GenerateSegmentID( segment, this, backwardPoint ) );
                backwardPoint->March( iInitiatorPoint, *segment );
                iSegmentIDList.pop_back();
            }
            else
            {
                if ( seekSegment( iSegmentIDList, GenerateSegmentID( segment, this, backwardPoint ) ) == false )
                {
                    uint64 loopID = FOdysseyVectorPathLoop::GenerateID( loopPointList, backwardPoint );

                    if ( segment->GetPath().GetPathLoopByID( loopID ) == nullptr )
                    {
                        FOdysseyVectorPathLoop *loopPath = new FOdysseyVectorPathLoop ( segment->GetPath(), loopID, loopPointList, backwardPoint );

                        segment->GetPath().AddLoop( loopPath );
                    } else printf("loop exists!!\n");
                }
            }
        }
    }

    loopPointList.pop_back ( );
}
#endif

FOdysseyVectorSegment*
FOdysseyVectorPoint::GetSegment( FOdysseyVectorPoint& iOtherPoint )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( ( ( segment->GetPoint(0) == this ) && ( segment->GetPoint(1) == &iOtherPoint ) ) 
          || ( ( segment->GetPoint(1) == this ) && ( segment->GetPoint(0) == &iOtherPoint ) ) )
        {
            return segment;
        }
    }

    return nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorPoint::GetOtherSegment( FOdysseyVectorSegment& iCurrentSegment )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( segment != &iCurrentSegment )
        {
            return segment;
        }
    }

    return nullptr;
}
