#include "OdysseyVectorVertex.h"

UOdysseyVectorVertex::~UOdysseyVectorVertex()
{
}

UOdysseyVectorVertex::UOdysseyVectorVertex()
    : FOdysseyVectorPoint()
   , mPath ( nullptr )
{
    SetVisited( false );
}

//static
UOdysseyVectorVertex*
UOdysseyVectorVertex::New( double iX, double iY, double iRadius )
{
    UOdysseyVectorVertex* vertex = NewObject<UOdysseyVectorVertex>();

    vertex->Init( iX, iY, iRadius );

    return vertex;
}

UOdysseyVectorPath* 
UOdysseyVectorVertex::GetPath()
{
    return mPath;
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
UOdysseyVectorVertex::SetRadius( double iRadius )
{
    FOdysseyVectorPoint::SetRadius( iRadius );

    InvalidateSegments();
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
UOdysseyVectorVertex::SetPath( UOdysseyVectorPath* iPath )
{
    mPath = iPath;
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

uint32
UOdysseyVectorVertex::GetSectionCount()
{
    return mSectionList.size();
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

FOdysseyVectorSection*
UOdysseyVectorVertex::GetLastSection()
{
    return mSectionList.back();
}

::ULIS::FVec2D
UOdysseyVectorVertex::GetVectorOnSegment( UOdysseyVectorSegment* iSegment, bool iNormalize )
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

FOdysseyVectorSection*
UOdysseyVectorVertex::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
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
UOdysseyVectorVertex::SetVisited( bool iVisited )
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
UOdysseyVectorVertex::IsVisited()
{
    return ( mFlags & VISITED ) ? true : false;
}

void
UOdysseyVectorVertex::SetInCycle( bool iInCycle, uint32 iCycleID )
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
UOdysseyVectorVertex::IsInCycle()
{
    return ( mFlags & INCYCLE ) ? true : false;
}

uint32
UOdysseyVectorVertex::GetCycleID()
{
    return mCycleID;
}

void
UOdysseyVectorVertex::SetMarched( bool iMarched )
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
UOdysseyVectorVertex::IsMarched()
{
    return ( mFlags & MARCHED ) ? true : false;
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

::ULIS::FVec2D&
UOdysseyVectorVertex::GetCoordsOnSegment( UOdysseyVectorSegment* iSegment )
{
    return GetCoords();
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

FOdysseyVectorSection*
UOdysseyVectorVertex::GetSection( UOdysseyVectorVertex& iOtherVertex )
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

FOdysseyVectorSection*
UOdysseyVectorVertex::GetOtherSection( FOdysseyVectorSection* iSection )
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
