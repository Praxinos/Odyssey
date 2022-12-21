#include "OdysseyVectorVertex.h"

UOdysseyVectorVertex::~UOdysseyVectorVertex()
{
}

UOdysseyVectorVertex::UOdysseyVectorVertex()
    : UOdysseyVectorPoint()
{
}

//static
UOdysseyVectorVertex*
UOdysseyVectorVertex::New( double iX, double iY, double iRadius )
{
    UOdysseyVectorVertex* vertex = NewObject<UOdysseyVectorVertex>();

    vertex->Init ( iX, iY, iRadius );

    return vertex;
}

void
UOdysseyVectorVertex::InvalidateSegments()
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        segment->Invalidate();
    }
}

void 
UOdysseyVectorVertex::SetX( double iX )
{
    mCoords.x  = iX;

    InvalidateSegments();
}

void 
UOdysseyVectorVertex::SetY( double iY )
{
    mCoords.y = iY;

    InvalidateSegments();
}

void 
UOdysseyVectorVertex::Set( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;

    InvalidateSegments();
}

void
UOdysseyVectorVertex::AddSegment( UOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );
}

void
UOdysseyVectorVertex::RemoveSegment( UOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );
}

uint32
UOdysseyVectorVertex::GetSegmentCount( )
{
    return mSegmentList.size();
}

std::list<UOdysseyVectorSegment*>&
UOdysseyVectorVertex::GetSegmentList()
{
    return mSegmentList;
}

UOdysseyVectorSegment*
UOdysseyVectorVertex::GetLastSegment()
{
    return mSegmentList.back();
}

UOdysseyVectorSegment*
UOdysseyVectorVertex::GetFirstSegment()
{
    return mSegmentList.front();
}

static uintptr_t
GenerateSegmentID( UOdysseyVectorSegment* iSegment, UOdysseyVectorVertex* iP0, UOdysseyVectorVertex* iP1 )
{
    uintptr_t xorVertex = reinterpret_cast<uintptr_t>(iP0) ^ reinterpret_cast<uintptr_t>(iP1);

    return xorVertex ^ reinterpret_cast<uintptr_t>(iSegment);
}

static bool seekVertex( std::list<UOdysseyVectorVertex*>& iVertexList
                     , UOdysseyVectorVertex* iVertex )
{
    for( std::list<UOdysseyVectorVertex*>::iterator it = iVertexList.begin(); it != iVertexList.end(); ++it )
    {
        if ( static_cast<UOdysseyVectorVertex*>(*it) == iVertex )
        {
            return true;
        }
    }

    return false;
}

double
UOdysseyVectorVertex::GetT( UOdysseyVectorSegment& iSegment )
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
UOdysseyVectorVertex::AddSection( FOdysseyVectorSection* iSection )
{
    mSectionList.push_back( iSection );
}

void
UOdysseyVectorVertex::RemoveSection( FOdysseyVectorSection* iSection )
{
    mSectionList.remove( iSection );
}

bool
UOdysseyVectorVertex::HasSegment( UOdysseyVectorSegment& iSegment )
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( segment == &iSegment )
        {
            return true;
        }
    }

    return false;
}

void
UOdysseyVectorVertex::AddLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.push_back( iLoop );
}

void
UOdysseyVectorVertex::RemoveLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.remove( iLoop );
}

void
UOdysseyVectorVertex::InvalidateLoops()
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        loop->Invalidate();
    }
}

bool
UOdysseyVectorVertex::IsClosestSection( FOdysseyVectorSection& iStartSection
                              , FOdysseyVectorSection& iEndSection )
{
    double startSectionP0T   = iStartSection.GetVertex( 0 )->GetT( iStartSection.GetSegment() );
    double startSectionP1T   = iStartSection.GetVertex( 1 )->GetT( iStartSection.GetSegment() );
    double startSectionT     = ( startSectionP0T + startSectionP1T ) * 0.5f;
    ::ULIS::FVec2D startSectionMidAt = iStartSection.GetSegment().GetPointAt( startSectionT );

    double endSectionP0T     = iEndSection.GetVertex( 0 )->GetT( iEndSection.GetSegment() );
    double endSectionP1T     = iEndSection.GetVertex( 1 )->GetT( iEndSection.GetSegment() );
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
                double p0T = section->GetVertex(0)->GetT(section->GetSegment());
                double p1T = section->GetVertex(1)->GetT(section->GetSegment());
                double t = ( p0T + p1T ) * 0.5f;
                ::ULIS::FVec2D pointAt = section->GetSegment().GetPointAt( t );
                ::ULIS::FVec3F originToVertex = { pointAt.x - this->GetCoords().x
                                               , pointAt.y - this->GetCoords().y, 0.0f };

               if ( originToVertex.DistanceSquared() )
               {
                    originToVertex.Normalize();

                    ::ULIS::FVec3F sectionCross = originToStartMid.CrossProduct( originToVertex );

                    // same side
                    if ( sectionCross.DotProduct( referenceCross ) > 0.0f )
                    {
                        if ( originToStartMid.DotProduct( originToVertex ) > originToStartMid.DotProduct( originToEndMid ) )
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
UOdysseyVectorVertex::GetRectangle()
{
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    bool inited = false;

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);
        ::ULIS::FRectD rect = segment->GetBoundingBox();

        bbox = ( inited == false ) ? rect : bbox | rect;

        inited = true;
    }

    return bbox;
}

::ULIS::FVec2D
UOdysseyVectorVertex::GetPosition( UOdysseyVectorSegment& iSegment )
{
    return mCoords;
}

std::list<FOdysseyVectorSection*>&
UOdysseyVectorVertex::GetSectionList()
{
    return mSectionList;
}

typedef struct _TREENODE
{
    struct _TREENODE* mParent;
    UOdysseyVectorVertex* mVertex;
    FOdysseyVectorSection* mSection;
    _TREENODE( struct _TREENODE* iParent, UOdysseyVectorVertex* iVertex, FOdysseyVectorSection* iSection )
        : mParent ( iParent )
        , mVertex ( iVertex )
        , mSection ( iSection ) {};
} TREENODE;

void TREENODETOLIST( TREENODE* node
                   , std::list<UOdysseyVectorVertex*>& iVertexList
                   , std::list<FOdysseyVectorSection*>& iSectionList )
{
    while ( node->mSection )
    {
        iVertexList.push_back ( node->mVertex );
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
UOdysseyVectorVertex::March()
{
    static int depth; // inits to zero by standard
    UOdysseyVectorVertex* sourceVertex = this;
    uint32 maxLoopcount = sourceVertex->GetSectionList().size();
    TREENODE *node = new TREENODE ( NULL, this, NULL );
    std::list<TREENODE*> nodeList;

    depth++;

    nodeList.push_back ( node );

    while ( nodeList.size() )
    {
        TREENODE *parentNode = nodeList.back();

        std::list<FOdysseyVectorSection*>& sectionList = parentNode->mVertex->GetSectionList();

        for( std::list<FOdysseyVectorSection*>::iterator it = sectionList.begin(); it != sectionList.end(); ++it )
        {
            FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

            if ( /*section != parentNode->mSection*/ TREENODESEEKSECTION( parentNode, section ) == false )
            {
                UOdysseyVectorVertex* nextVertex = ( parentNode->mVertex == section->GetVertex(0) ) ? section->GetVertex(1) : section->GetVertex(0);
                TREENODE* nextNode = new TREENODE ( parentNode, nextVertex, section );

                if ( nextVertex == this )
                {
                    UOdysseyVectorPath* path = nextNode->mSection->GetSegment().GetPath();
                    std::list<UOdysseyVectorVertex*> loopVertexList;
                    std::list<FOdysseyVectorSection*> loopSectionList;
                    uint64 loopID;

                    TREENODETOLIST ( nextNode, loopVertexList, loopSectionList );
printf("loop detected\n");
                    loopID = UOdysseyVectorLoop::GenerateID( loopSectionList );

                    if ( path->GetLoopByID( loopID ) == nullptr )
                    {
                        if ( IsClosestSection ( *loopSectionList.front(), *loopSectionList.back() ) == true )
                        {
                            UOdysseyVectorLoop *loop = UOdysseyVectorLoop::New( path
                                                                              , loopID
                                                                              , nextNode->mVertex
                                                                              , loopVertexList
                                                                              , loopSectionList );

                            path->AddLoop( loop );
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
UOdysseyVectorVertex::March( UOdysseyVectorVertexIntersection& iInitiatorVertex
                           , UOdysseyVectorSegment& iCurrentSegment )
{
    static std::list<UOdysseyVectorVertex*> loopVertexList;
    static std::list<uintptr_t> iSegmentIDList;

    loopVertexList.push_back ( this );

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);
        double t =  static_cast<UOdysseyVectorVertexIntersection*>(this)->GetT(*segment);
        UOdysseyVectorVertex* forwardVertex = segment->GetNextVertex(t);
        UOdysseyVectorVertex* backwardVertex = segment->GetPreviousVertex(t);

        if ( forwardVertex )
        {
            if ( seekVertex( loopVertexList, forwardVertex ) == false )
            {
                iSegmentIDList.push_back( GenerateSegmentID( segment, this, forwardVertex) );
                forwardVertex->March( iInitiatorVertex, *segment );
                iSegmentIDList.pop_back();
            }
            else
            {
                if ( seekSegment( iSegmentIDList, GenerateSegmentID( segment, this, forwardVertex ) ) == false )
                {
                    uint64 loopID = FOdysseyVectorPathLoop::GenerateID( loopVertexList, forwardVertex );

                    if ( segment->GetPath().GetPathLoopByID( loopID ) == nullptr )
                    {
                        FOdysseyVectorPathLoop *loopPath = new FOdysseyVectorPathLoop ( segment->GetPath(), loopID, loopVertexList, forwardVertex );

                        segment->GetPath().AddLoop( loopPath );
                    } else printf("loop exists!!\n");
                }
            }
        }

        if( backwardVertex )
        {
            if( seekVertex( loopVertexList, backwardVertex ) == false )
            {
                iSegmentIDList.push_back( GenerateSegmentID( segment, this, backwardVertex ) );
                backwardVertex->March( iInitiatorVertex, *segment );
                iSegmentIDList.pop_back();
            }
            else
            {
                if ( seekSegment( iSegmentIDList, GenerateSegmentID( segment, this, backwardVertex ) ) == false )
                {
                    uint64 loopID = FOdysseyVectorPathLoop::GenerateID( loopVertexList, backwardVertex );

                    if ( segment->GetPath().GetPathLoopByID( loopID ) == nullptr )
                    {
                        FOdysseyVectorPathLoop *loopPath = new FOdysseyVectorPathLoop ( segment->GetPath(), loopID, loopVertexList, backwardVertex );

                        segment->GetPath().AddLoop( loopPath );
                    } else printf("loop exists!!\n");
                }
            }
        }
    }

    loopVertexList.pop_back ( );
}
#endif

UOdysseyVectorSegment*
UOdysseyVectorVertex::GetSegment( UOdysseyVectorVertex& iOtherVertex )
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( ( ( segment->GetPoint(0) == this ) && ( segment->GetPoint(1) == &iOtherVertex ) ) 
          || ( ( segment->GetPoint(1) == this ) && ( segment->GetPoint(0) == &iOtherVertex ) ) )
        {
            return segment;
        }
    }

    return nullptr;
}

UOdysseyVectorSegment*
UOdysseyVectorVertex::GetOtherSegment( UOdysseyVectorSegment& iCurrentSegment )
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        if ( segment != &iCurrentSegment )
        {
            return segment;
        }
    }

    return nullptr;
}
