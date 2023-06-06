#include "OdysseyVectorVertex.h"

FOdysseyVectorVertex::~FOdysseyVectorVertex()
{
}

FOdysseyVectorVertex::FOdysseyVectorVertex( FOdysseyVectorPath* iPath, double iX, double iY, double iRadius )
    : FOdysseyVectorPoint()
   , mPath ( iPath )
   , mNearestSegment( nullptr )
   , mNearestVertex( nullptr )
{
    mCtrlPoint = FOdysseyVectorHandlePoint::New( this );

    Init( iX, iY, iRadius );
}

void
FOdysseyVectorVertex::SetNearestVertex( FOdysseyVectorVertex* iNearestVertex )
{
    mNearestVertex = iNearestVertex;
}

FOdysseyVectorVertex*
FOdysseyVectorVertex::GetNearestVertex()
{
    return mNearestVertex;
}

FOdysseyVectorPath* 
FOdysseyVectorVertex::GetPath()
{
    return mPath;
}

void
FOdysseyVectorVertex::BuildExplorationPairs( std::vector<FExplorationPair>& iExplorationPairsArray )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* returnSection = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorSection* departSection = GetCycleNextSection( returnSection, 1.0f );

        iExplorationPairsArray.push_back( FExplorationPair( returnSection, this, departSection ) );
    }
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
{
    uint32 sectionCount = mSectionList.size();

    if( mSectionList.size() == 2 )
    {
        return GetOtherSection( iLastSection, false );
    }

    if( mSectionList.size() > 2 )
    {
        ::ULIS::FVec2D lastSectionVector = iLastSection->GetVectorFromVertex( this, false, true );
        FOdysseyVectorSection* minDotSection = nullptr;
        FOdysseyVectorSection* maxDotSection = nullptr;
        ::ULIS::FVec2D minDotSectionVector;
        ::ULIS::FVec2D maxDotSectionVector;
        double minDot =  DBL_MAX;
        double maxDot = -DBL_MAX;
        std::vector<FOdysseyVectorSection*> rightSideSection;
        std::vector<FOdysseyVectorSection*> wrongSideSection;

        rightSideSection.reserve( sectionCount );
        wrongSideSection.reserve( sectionCount );

        for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
        {
            FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);
            ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( this, false, true );

            if( section != iLastSection )
            {
                if( ( FOdysseyVector::Cross2D( -lastSectionVector, sectionVector ) * iOrientation > 0.0f ) )
                {
                    rightSideSection.push_back( section );
                }
                else
                {
                    wrongSideSection.push_back( section );
                }
            }
        }

        for( int i = 0; i < rightSideSection.size(); i++ )
        {
            FOdysseyVectorSection* section = rightSideSection[i];
            ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( this, false, true );
            double dot = lastSectionVector.DotProduct( sectionVector );

            if( dot > maxDot )
            {
                maxDotSection = section;
                maxDotSectionVector = sectionVector;
                maxDot = dot;
            }
        }

        if( maxDotSection )
        {
            return maxDotSection;
        }

        for( int i = 0; i < wrongSideSection.size(); i++ )
        {
            FOdysseyVectorSection* section = wrongSideSection[i];
            ::ULIS::FVec2D sectionVector = section->GetVectorFromVertex( this, false, true );
            double dot = lastSectionVector.DotProduct( sectionVector );

            if( dot < minDot )
            {
                minDotSection = section;
                minDotSectionVector = sectionVector;
                minDot = dot;
            }
        }

        if( minDotSection )
        {
            return minDotSection;
        }
    }

    return nullptr;
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize )
{
    ::ULIS::FVec2D vec;

    vec = ( iSegment->GetVertex(0) == this ) ? iSegment->GetPointAt(0.01f) - iSegment->GetPointAt(0.0f)
                                             : iSegment->GetPointAt(0.99f) - iSegment->GetPointAt(1.0f);

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

bool
FOdysseyVectorVertex::IsSmooth()
{
    if( mSegmentList.size() == 2 )
    {
        ::ULIS::FVec2D firstSegmentVector = GetVectorOnSegment( GetFirstSegment(), true );
        ::ULIS::FVec2D lastSegmentVector = GetVectorOnSegment( GetLastSegment(), true );
        double dot = firstSegmentVector.DotProduct( lastSegmentVector );

        if( fabs(dot) > 0.99f )
        {
            return true;
        }
    }

    return false;
}

FOdysseyVectorHandlePoint*
FOdysseyVectorVertex::GetHandle()
{
    return mCtrlPoint;
}

double
FOdysseyVectorVertex::GetT( FOdysseyVectorSegment* iSegment )
{
    return ( this == iSegment->GetVertex(0) ) ? 0.0f : 1.0f;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetNearestSegment()
{
    return mNearestSegment;
}

double
FOdysseyVectorVertex::GetDistanceToNearestSegment()
{
    return mDistanceToNearestSegment;
}

double
FOdysseyVectorVertex::GetNearestSegmentT()
{
    return mNearestSegmentT;
}

void
FOdysseyVectorVertex::SetNearestSegment( FOdysseyVectorSegment* iNearestSegment
                                       , double iDistanceToNearestSegment
                                       , double iNearestSegmentT )
{
    mNearestSegment = iNearestSegment;
    mDistanceToNearestSegment = iDistanceToNearestSegment;
    mNearestSegmentT = iNearestSegmentT;
    mNearestVertex = nullptr;
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
FOdysseyVectorVertex::SetPath( FOdysseyVectorPath* iPath )
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
/*
::ULIS::FVec2D
FOdysseyVectorVertex::GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize )
{
    ::ULIS::FVec2D vec;

    vec = ( iSegment->GetVertex(0) == this ) ? iSegment->GetVertex(1)->GetCoords() -
                                                                       GetCoords()
                                              :                        GetCoords() -
                                               iSegment->GetVertex(0)->GetCoords();

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}
*/
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
FOdysseyVectorVertex::AddSection( FOdysseyVectorSection* iSection )
{
    mSectionList.push_back( iSection );
}

void
FOdysseyVectorVertex::RemoveSection( FOdysseyVectorSection* iSection )
{
    mSectionList.remove( iSection );
}

bool
FOdysseyVectorVertex::HasSegment( FOdysseyVectorSegment* iSegment )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( segment == iSegment )
        {
            return true;
        }
    }

    return false;
}

::ULIS::FRectD
FOdysseyVectorVertex::GetBoundingBox( bool iWorld )
{
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    if( iWorld )
    {
        BLPoint pt = GetPath()->GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );

        bbox.x = pt.x;
        bbox.y = pt.y;
        bbox.w = 1;
        bbox.h = 1;
    }
    else
    {
        bbox.x = mCoords.x;
        bbox.y = mCoords.y;
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);
        ::ULIS::FRectD rect = segment->GetBoundingBox( iWorld );

        bbox = bbox | rect;
    }

    return bbox;
}

std::list<FOdysseyVectorSection*>&
FOdysseyVectorVertex::GetSectionList()
{
    return mSectionList;
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetSection( FOdysseyVectorSegment* iSegment )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* section = static_cast<FOdysseyVectorSection*>(*it);

        if( section->GetSegment() == iSegment )
        {
            return section;
        }
    }

    return nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetSegment( FOdysseyVectorVertex* iOtherVertex )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if ( ( ( segment->GetPoint(0) == this ) && ( segment->GetPoint(1) == iOtherVertex ) ) 
          || ( ( segment->GetPoint(1) == this ) && ( segment->GetPoint(0) == iOtherVertex ) ) )
        {
            return segment;
        }
    }

    return nullptr;
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetOtherSection( FOdysseyVectorSection* iSection, bool iSameSegment )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* otherSection = static_cast<FOdysseyVectorSection*>(*it);

        if( otherSection != iSection )
        {
            if( iSameSegment == true )
            {
                if ( otherSection->GetSegment() == iSection->GetSegment() )
                {
                    return otherSection;
                }
            }
            else
            {
                return otherSection;
            }
        }
    }

    return nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetOtherSegment( FOdysseyVectorSegment* iSegment )
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* otherSegment = static_cast<FOdysseyVectorSegment*>(*it);

        if( otherSegment != iSegment )
        {
            return otherSegment;
        }
    }

    return nullptr;
}
