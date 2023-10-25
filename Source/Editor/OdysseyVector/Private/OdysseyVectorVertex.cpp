#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorHandleSegment.h"

FOdysseyVectorVertex::~FOdysseyVectorVertex()
{
}

FOdysseyVectorVertex::FOdysseyVectorVertex( FOdysseyVectorPath* iPath, double iX, double iY, double iRadius )
    : FOdysseyVectorPoint( iX, iY, iRadius )
   , mPath ( iPath )
   , mFlags( 0 )
   , mNearestSegment( nullptr )
   , mNearestVertex( nullptr )
{
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetWorldCoords()
{
    BLPoint worldPoint = GetPath()->GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );

    return ::ULIS::FVec2D( worldPoint.x, worldPoint.y );
}

// static
bool
FOdysseyVectorVertex::GetMinMaxFromList( std::list<FOdysseyVectorVertex*>& iVertexList
                                       , double& oXMin
                                       , double& oYMin
                                       , double& oXMax
                                       , double& oYMax )
{
    if( iVertexList.size() )
    {
        FOdysseyVectorVertex* firstVertex = iVertexList.front();
        ::ULIS::FVec2D& firstVertexCoords = firstVertex->GetCoords();
        std::list<FOdysseyVectorVertex*>::iterator it;

        oXMin = oXMax = firstVertexCoords.x;
        oYMin = oYMax = firstVertexCoords.y;

        for( it = iVertexList.begin(); it != iVertexList.end(); ++it )
        {
            FOdysseyVectorVertex* vertex = *it;
            ::ULIS::FVec2D& vertexCoords = vertex->GetCoords();

            if( vertexCoords.x < oXMin ) oXMin = vertexCoords.x;
            if( vertexCoords.y < oYMin ) oYMin = vertexCoords.y;
            if( vertexCoords.x > oXMax ) oXMax = vertexCoords.x;
            if( vertexCoords.y > oYMax ) oYMax = vertexCoords.y;
        }

        return true;
    }

    return false;
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

FOdysseyVectorSection*
FOdysseyVectorVertex::GetCycleNextSection( FOdysseyVectorSection* iLastSection, double iOrientation )
{
    uint32 sectionCount = mSectionList.size();

    if( sectionCount == 2 )
    {
        return GetOtherSection( iLastSection, false );
    }

    if( sectionCount > 2 )
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

void
FOdysseyVectorVertex::BuildExplorationPairs( std::vector<FExplorationPair>& oExplorationPairsArray )
{
    for( std::list<FOdysseyVectorSection*>::iterator it = mSectionList.begin(); it != mSectionList.end(); ++it )
    {
        FOdysseyVectorSection* returnSection = static_cast<FOdysseyVectorSection*>(*it);
        FOdysseyVectorSection* departSection = GetCycleNextSection( returnSection, 1.0f );

        oExplorationPairsArray.push_back( FExplorationPair( returnSection, this, departSection ) );
    }
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetAverageVectorOnSegmentHandle( bool iNormalize )
{
    uint32 segmentCount = GetSegmentCount();
    ::ULIS::FVec2D averageVector( 0.0f, 0.0f );

    if( segmentCount )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegment* segment = (*it);

            averageVector += segment->GetHandleVector( this, true );
        }

        averageVector.x /= segmentCount;
        averageVector.y /= segmentCount;

        if ( iNormalize && averageVector.DistanceSquared() )
        {
            averageVector.Normalize();
        }
    }

    return averageVector;
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetAverageStraightVectorOnSegment( bool iNormalize )
{
    uint32 segmentCount = GetSegmentCount();
    ::ULIS::FVec2D averageVector( 0.0f, 0.0f );

    if( segmentCount )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegment* segment = (*it);

            averageVector += segment->GetVector( this, true );
        }

        averageVector.x /= segmentCount;
        averageVector.y /= segmentCount;

        if ( iNormalize && averageVector.DistanceSquared() )
        {
            averageVector.Normalize();
        }
    }

    return averageVector;
}

//static
void
FOdysseyVectorVertex::ArrayToSegmentArray( std::vector<FOdysseyVectorVertex*>& iVertexArray
                                         , std::vector<FOdysseyVectorSegment*>& oSegmentArray )
{

    if( iVertexArray.size() )
    {
        oSegmentArray.reserve( iVertexArray.size() );

        for( int i = 0; i < iVertexArray.size(); i++ )
        {
            FOdysseyVectorVertex* vertex = iVertexArray[i];
            std::list<FOdysseyVectorSegment*>& segmentList = vertex->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
            {
                FOdysseyVectorSegment* segment = (*it);

                if( std::find( oSegmentArray.begin(), oSegmentArray.end(), segment ) == oSegmentArray.end() )
                {
                    oSegmentArray.push_back( segment );
                }
            }
        }
    }
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetAverageVectorOnSegment( bool iNormalize )
{
    uint32 segmentCount = GetSegmentCount();
    ::ULIS::FVec2D averageVector( 0.0f, 0.0f );

    if( segmentCount )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegment* segment = (*it);

            averageVector += GetVectorOnSegment( segment, true );
        }

        averageVector.x /= segmentCount;
        averageVector.y /= segmentCount;

        if ( iNormalize && averageVector.DistanceSquared() )
        {
            averageVector.Normalize();
        }
    }

    return averageVector;
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize )
{
    ::ULIS::FVec2D vec;

    vec = ( iSegment->GetVertex(0) == this ) ?  iSegment->GetTangentAt( 0.0f, iNormalize )
                                             : -iSegment->GetTangentAt( 1.0f, iNormalize );

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

        if( dot < -0.9999f )
        {
            return true;
        }
    }

    return false;
}

double
FOdysseyVectorVertex::GetT( FOdysseyVectorSegment* iSegment )
{
    return ( this == iSegment->GetVertex(0) ) ? 0.0f : 1.0f;
}

double
FOdysseyVectorVertex::GetT( FOdysseyVectorSection* iSection )
{
    return ( this == iSection->GetVertex(0) ) ? 0.0f : 1.0f;
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
FOdysseyVectorVertex::SetCoords( double iX, double iY, double iRadius )
{
    if( iRadius < 0.0f )
    {
        iRadius = 0.0f;
    }

    FOdysseyVectorPoint::SetCoords( iX, iY, iRadius );

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
FOdysseyVectorVertex::SetSelected( bool iSelected )
{
    if( iSelected == true )
    {
        mFlags |= SELECTED;
    }
    else
    {
        mFlags &= (~SELECTED);
    }
}

bool
FOdysseyVectorVertex::IsSelected()
{
    return ( mFlags & SELECTED ) ? true : false;
}

void
FOdysseyVectorVertex::SetChained( bool iChained )
{
    if( iChained == true )
    {
        mFlags |= CHAINED;
    }
    else
    {
        mFlags &= (~CHAINED);
    }
}

bool
FOdysseyVectorVertex::IsChained()
{
    return ( mFlags & CHAINED ) ? true : false;
}

void
FOdysseyVectorVertex::AlignHandles( FOdysseyVectorHandleSegment* iHandle )
{
    ::ULIS::FVec2D handleVector = iHandle->GetCoords() - GetCoords();

    if( handleVector.Distance() )
    {
        FOdysseyVectorSegment* segment = iHandle->GetOwner();
        FOdysseyVectorSegment* otherSegment = GetOtherSegment( segment );

        handleVector.Normalize();

        if( otherSegment )
        {
            if( otherSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* otherCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(otherSegment);
                FOdysseyVectorHandleSegment* otherHandle = otherCubicSegment->GetHandle( this );
                ::ULIS::FVec2D otherHandleVector = otherHandle->GetCoords() - GetCoords();

                otherHandle->Set( GetCoords() - ( otherHandleVector.Distance() * handleVector ) );
            }
        }
    }
}

void
FOdysseyVectorVertex::SetHandleAligned( bool iHandleAligned )
{
    if( iHandleAligned == true )
    {
        mFlags |= HANDLE_ALIGNED;
    }
    else
    {
        mFlags &= (~HANDLE_ALIGNED);
    }
}

bool
FOdysseyVectorVertex::IsHandleAligned()
{
    return ( mFlags & HANDLE_ALIGNED ) ? true : false;
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

FOdysseyVectorHandleSegment*
FOdysseyVectorVertex::GetOtherSegmentHandle( FOdysseyVectorSegment* iSegment )
{
    FOdysseyVectorSegment* otherSegment = GetOtherSegment( iSegment );

    if( otherSegment )
    {
        if( otherSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* otherCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(otherSegment);

            return otherCubicSegment->GetHandle( this );
        }
    }

    return nullptr;
}

FOdysseyVectorJoint&
FOdysseyVectorVertex::GetJoint()
{
    return mJoint;
}

double
FOdysseyVectorVertex::GetJointLength()
{
    return mJoint.GetLength();
}

void
FOdysseyVectorVertex::DrawJoint( BLContext* iBLContext, uint64 iDrawingFlags )
{
    mJoint.Draw( iBLContext, iDrawingFlags );
}

void
FOdysseyVectorVertex::MakeJoint( FOdysseyVectorSegment* iPreviousSegment )
{
    if( iPreviousSegment )
    {
        FOdysseyVectorSegment* nextSegment = GetOtherSegment( iPreviousSegment );

        if( iPreviousSegment && nextSegment )
        {
            ::ULIS::FVec2D segment0Vector = GetVectorOnSegment( iPreviousSegment, false );
            ::ULIS::FVec2D segment1Vector = GetVectorOnSegment( nextSegment     , false );

            if( segment0Vector.DistanceSquared() )
            {
                segment0Vector.Normalize();
            }

            if( segment1Vector.DistanceSquared() )
            {
                segment1Vector.Normalize();
            }

            switch( mPath->mPathParam.JointType )
            {
                case eJointType::Linear :
                    mJoint.MakeLinear( mCoords
                                     , segment0Vector
                                     , segment1Vector
                                     , mRadius );
                break;

                case eJointType::Miter :
                    mJoint.MakeMiter( mCoords
                                    , segment0Vector
                                    , segment1Vector
                                    , mRadius
                                    , mPath->mPathParam.MiterLimit );
                break;

                case eJointType::Radial :
                    mJoint.MakeRadial( mCoords
                                     , segment0Vector
                                     , segment1Vector
                                     , mRadius );
                break;

                default:
                    mJoint.MakeNone();
                break;
            }

            return;
        }
    }

    mJoint.MakeNone();
}
