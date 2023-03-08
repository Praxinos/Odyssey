#include "OdysseyVectorVertex.h"

FOdysseyVectorVertex::~FOdysseyVectorVertex()
{
}

FOdysseyVectorVertex::FOdysseyVectorVertex()
    : FOdysseyVectorPoint()
   , mPath ( nullptr )
{
    SetVisited( false );
}

//static
FOdysseyVectorVertex*
FOdysseyVectorVertex::New( double iX, double iY, double iRadius )
{
    FOdysseyVectorVertex* vertex = NewObject<FOdysseyVectorVertex>();

    vertex->Init( iX, iY, iRadius );

    return vertex;
}

UOdysseyVectorPath* 
FOdysseyVectorVertex::GetPath()
{
    return mPath;
}

void
FOdysseyVectorVertex::InvalidateSegments()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        segment->Invalidate();
    }
}

void
FOdysseyVectorVertex::SetRadius( double iRadius )
{
    FOdysseyVectorPoint::SetRadius( iRadius );

    InvalidateSegments();
}

void 
FOdysseyVectorVertex::SetX( double iX )
{
    mCoords.x  = iX;

    InvalidateSegments();
}

void 
FOdysseyVectorVertex::SetY( double iY )
{
    mCoords.y = iY;

    InvalidateSegments();
}

void 
FOdysseyVectorVertex::Set( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;

    InvalidateSegments();
}

void
FOdysseyVectorVertex::SetPath( UOdysseyVectorPath* iPath )
{
    mPath = iPath;
}

void
FOdysseyVectorVertex::AddSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );
}

void
FOdysseyVectorVertex::RemoveSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );
}

uint32
FOdysseyVectorVertex::GetSegmentCount( )
{
    return mSegmentList.size();
}

uint32
FOdysseyVectorVertex::GetSectionCount()
{
    return mSectionList.size();
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorVertex::GetSegmentList()
{
    return mSegmentList;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetLastSegment()
{
    return mSegmentList.back();
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetFirstSegment()
{
    return mSegmentList.front();
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetLastSection()
{
    return mSectionList.back();
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize )
{
    ::ULIS::FVec2D vec;

    vec = ( iSegment->GetVertex(0) == this ) ? iSegment->GetVertex(1)->GetCoordsOnSegment(iSegment) -
                                                                       GetCoordsOnSegment(iSegment)
                                              :                        GetCoordsOnSegment(iSegment) -
                                               iSegment->GetVertex(0)->GetCoordsOnSegment(iSegment);

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

static uintptr_t
GenerateSegmentID( FOdysseyVectorSegment* iSegment, FOdysseyVectorVertex* iP0, FOdysseyVectorVertex* iP1 )
{
    uintptr_t xorVertex = reinterpret_cast<uintptr_t>(iP0) ^ reinterpret_cast<uintptr_t>(iP1);

    return xorVertex ^ reinterpret_cast<uintptr_t>(iSegment);
}

static bool seekVertex( std::list<FOdysseyVectorVertex*>& iVertexList
                     , FOdysseyVectorVertex* iVertex )
{
    for( std::list<FOdysseyVectorVertex*>::iterator it = iVertexList.begin(); it != iVertexList.end(); ++it )
    {
        if ( static_cast<FOdysseyVectorVertex*>(*it) == iVertex )
        {
            return true;
        }
    }

    return false;
}

double
FOdysseyVectorVertex::GetT( FOdysseyVectorSegment& iSegment )
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

FOdysseyVectorSection*
FOdysseyVectorVertex::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
{
    ::ULIS::FVec2D lastSectionVector = -iLastSection->GetVectorFromVertex( this );

    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        if( section->GetSegment() != iLastSection->GetSegment() )
        {
            ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( this );

            if( ( FOdysseyVector::Cross2D( lastSectionVector, sectionVector ) * iOrientation > 0.0f ) )
            {
                return section;
            }
        }
    }

    return /*GetOtherSection( iLastSection )*/nullptr;
}

void
FOdysseyVectorVertex::SetVisited( bool iVisited )
{
    if( iVisited == true )
    {
        mFlags |= VISITED;
    }
    else
    {
        mFlags &= (~VISITED);
    }
}

bool
FOdysseyVectorVertex::IsVisited()
{
    return ( mFlags & VISITED ) ? true : false;
}

void
FOdysseyVectorVertex::SetInCycle( bool iInCycle, uint32 iCycleID )
{
    if( iInCycle == true )
    {
        mFlags |= INCYCLE;
    }
    else
    {
        mFlags &= (~INCYCLE);
    }

    mCycleID = iCycleID;
}

bool
FOdysseyVectorVertex::IsInCycle()
{
    return ( mFlags & INCYCLE ) ? true : false;
}

uint32
FOdysseyVectorVertex::GetCycleID()
{
    return mCycleID;
}

void
FOdysseyVectorVertex::SetMarched( bool iMarched )
{
    if( iMarched == true )
    {
        mFlags |= MARCHED;
    }
    else
    {
        mFlags &= (~MARCHED);
    }
}

bool
FOdysseyVectorVertex::IsMarched()
{
    return ( mFlags & MARCHED ) ? true : false;
}

void
FOdysseyVectorVertex::AddSection( FOdysseyVectorSection* iSection )
{
    mSectionList.push_back( iSection );
}

void
FOdysseyVectorVertex::RemoveSection( FOdysseyVectorSection* iSection )
{
    mSectionList.remove( iSection );
}

::ULIS::FVec2D&
FOdysseyVectorVertex::GetCoordsOnSegment( FOdysseyVectorSegment* iSegment )
{
    return GetCoords();
}

bool
FOdysseyVectorVertex::HasSegment( FOdysseyVectorSegment& iSegment )
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

::ULIS::FRectD
FOdysseyVectorVertex::GetRectangle()
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
FOdysseyVectorVertex::GetPosition( FOdysseyVectorSegment& iSegment )
{
    return mCoords;
}

std::list<FOdysseyVectorSection*>&
FOdysseyVectorVertex::GetSectionList()
{
    return mSectionList;
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetSection( FOdysseyVectorVertex& iOtherVertex )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        if ( ( ( section->GetVertex(0) == this ) && ( section->GetVertex(1) == &iOtherVertex ) ) 
          || ( ( section->GetVertex(1) == this ) && ( section->GetVertex(0) == &iOtherVertex ) ) )
        {
            return section;
        }
    }

    return nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetSegment( FOdysseyVectorVertex& iOtherVertex )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( ( ( segment->GetPoint(0) == this ) && ( segment->GetPoint(1) == &iOtherVertex ) ) 
          || ( ( segment->GetPoint(1) == this ) && ( segment->GetPoint(0) == &iOtherVertex ) ) )
        {
            return segment;
        }
    }

    return nullptr;
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetOtherSection( FOdysseyVectorSection* iSection )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* otherSection = static_cast<FOdysseyVectorSection*>(*it);

        if( otherSection != iSection )
        {
            if ( otherSection->GetSegment() == iSection->GetSegment() )
            {
                return otherSection;
            }
        }
    }

    return nullptr;
}


FOdysseyVectorSegment*
FOdysseyVectorVertex::GetOtherSegment( FOdysseyVectorSegment& iCurrentSegment )
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
