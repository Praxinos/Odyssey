#include "OdysseyVectorVertex.h"

UOdysseyVectorVertex::~UOdysseyVectorVertex()
{
}

UOdysseyVectorVertex::UOdysseyVectorVertex()
    : UOdysseyVectorPoint()
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
    UOdysseyVectorPoint::SetRadius( iRadius );

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

            if( ( FOdysseyVector::Cross2D( sectionVector, lastSectionVector ) * iOrientation > 0.0f ) // same side
              || ( mSectionList.size() == 2 ) ) // no other choice
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

bool
UOdysseyVectorVertex::IsClosestSection( FOdysseyVectorSection& iStartSection
                                      , FOdysseyVectorSection& iEndSection )
{
    double startSectionP0T   = iStartSection.GetVertex( 0 )->GetT( *iStartSection.GetSegment() );
    double startSectionP1T   = iStartSection.GetVertex( 1 )->GetT( *iStartSection.GetSegment() );
    double startSectionT     = ( startSectionP0T + startSectionP1T ) * 0.5f;
    ::ULIS::FVec2D startSectionMidAt = iStartSection.GetSegment()->GetPointAt( startSectionT );

    double endSectionP0T     = iEndSection.GetVertex( 0 )->GetT( *iEndSection.GetSegment() );
    double endSectionP1T     = iEndSection.GetVertex( 1 )->GetT( *iEndSection.GetSegment() );
    double endSectionT       = ( endSectionP0T + endSectionP1T ) * 0.5f;
    ::ULIS::FVec2D endSectionMidAt   = iEndSection.GetSegment()->GetPointAt( endSectionT );

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
                double p0T = section->GetVertex(0)->GetT( *section->GetSegment());
                double p1T = section->GetVertex(1)->GetT( *section->GetSegment());
                double t = ( p0T + p1T ) * 0.5f;
                ::ULIS::FVec2D pointAt = section->GetSegment()->GetPointAt( t );
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
