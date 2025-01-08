// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorIntersection.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVector.h"

FOdysseyVectorVertex::~FOdysseyVectorVertex()
{
}

FOdysseyVectorVertex::FOdysseyVectorVertex( double iX, double iY, double iRadius )
    : FOdysseyVectorPoint( iX, iY )
    , mJoint( this )
    , mOwner ( nullptr )
    , mFlags( 0 )
    , mRadius ( iRadius )
    , mNearestSegment( nullptr )
    , mNearestVertex( nullptr )
{
    mJoint.ResetBBox();
}

bool
FOdysseyVectorVertex::HasSegment( FOdysseyVectorSegment* iSegment )
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        if( segment == iSegment )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorVertex::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FOdysseyVectorVertex::GetID()
{
    return mID;
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetWorldCoords()
{
    BLPoint worldPoint = mOwner->GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );

    return ::ULIS::FVec2D( worldPoint.x, worldPoint.y );
}

// static
bool
FOdysseyVectorVertex::GetMinMaxFromList( std::list<FOdysseyVectorVertex*>& iVertexList
                                       , bool iWithHandles
                                       , double& oXMin
                                       , double& oYMin
                                       , double& oXMax
                                       , double& oYMax )
{
    if( iVertexList.size() )
    {
        FOdysseyVectorVertex* firstVertex = iVertexList.front();
        ::ULIS::FVec2D& firstVertexCoords = firstVertex->GetCoords();

        oXMin = oXMax = firstVertexCoords.x;
        oYMin = oYMax = firstVertexCoords.y;

        for( FOdysseyVectorVertex* vertex : iVertexList )
        {
            ::ULIS::FVec2D& vertexCoords = vertex->GetCoords();

            if( vertexCoords.x < oXMin ) oXMin = vertexCoords.x;
            if( vertexCoords.y < oYMin ) oYMin = vertexCoords.y;
            if( vertexCoords.x > oXMax ) oXMax = vertexCoords.x;
            if( vertexCoords.y > oYMax ) oYMax = vertexCoords.y;

            if( iWithHandles == true )
            {
                for( FOdysseyVectorSegment* segment : vertex->GetSegmentList() )
                {
                    FOdysseyVectorHandleSegment* handle = segment->GetHandle( vertex );
                    ::ULIS::FVec2D& handleCoords = handle->GetCoords();

                    if( handleCoords.x < oXMin ) oXMin = handleCoords.x;
                    if( handleCoords.y < oYMin ) oYMin = handleCoords.y;
                    if( handleCoords.x > oXMax ) oXMax = handleCoords.x;
                    if( handleCoords.y > oYMax ) oYMax = handleCoords.y;
                }
            }
        }

        return true;
    }

    return false;
}

FOdysseyVectorObject*
FOdysseyVectorVertex::GetOwner()
{
    return mOwner;
}

FOdysseyVectorPath*
FOdysseyVectorVertex::GetOwnerAsPath()
{
    return static_cast<FOdysseyVectorPath*>(mOwner);
}

FSectionLinkInfo*
FOdysseyVectorVertex::GetSectionLinkInfo( FOdysseyVectorSegment* iSegment )
{
    for( FSectionLinkInfo& sectionLinkInfo : mSectionLinkInfoList )
    {
        if( sectionLinkInfo.section->GetSegment() == iSegment )
        {
            return &sectionLinkInfo;
        }
    }

    return nullptr;
}

FSectionLinkInfo*
FOdysseyVectorVertex::GetSectionLinkInfo( FOdysseyVectorSection* iSection
                                        , uint32 iSectionVertexIndex )
{
    for( FSectionLinkInfo& sectionLinkInfo : mSectionLinkInfoList )
    {
        if( ( sectionLinkInfo.section            == iSection            )
         && ( sectionLinkInfo.sectionVertexIndex == iSectionVertexIndex ) )
        {
            return &sectionLinkInfo;
        }
    }

    return nullptr;
}

static FSectionLinkInfo*
FindNextSectionLinkInfo( FSectionLinkInfo* iLastSectionLinkInfo
                       , std::list<FSectionLinkInfo>& iCandidateSectionList
                       , double iOrientation )
{
    FSectionLinkInfo* rightRet = nullptr;
    FSectionLinkInfo* wrongRet = nullptr;

    if( iCandidateSectionList.size() )
    {
        ::ULIS::FVec2D lastSectionVector = iLastSectionLinkInfo->GetVector();
        double minDot =  DBL_MAX;
        double maxDot = -DBL_MAX;
        std::vector<FSectionLinkInfo*> rightSideSection;
        std::vector<FSectionLinkInfo*> wrongSideSection;
        uint32 sectionCount = iCandidateSectionList.size();

        rightSideSection.reserve( sectionCount );
        wrongSideSection.reserve( sectionCount );

        for( FSectionLinkInfo& candidateSectionInfo : iCandidateSectionList )
        {
            if( &candidateSectionInfo != iLastSectionLinkInfo )
            {
                if( candidateSectionInfo.GetVector().DistanceSquared() )
                {
                    double crossProduct = FOdysseyVector::Cross2D( -lastSectionVector
                                                                  , candidateSectionInfo.GetVector() );

                    if( ( crossProduct * iOrientation >= 0.0f ) )
                    {
                        rightSideSection.emplace_back( &candidateSectionInfo );
                    }

                    if( ( crossProduct * iOrientation <= 0.0f ) )
                    {
                        wrongSideSection.emplace_back( &candidateSectionInfo );
                    }
                }
            }
        }

        for( int i = 0; i < rightSideSection.size(); i++ )
        {
            FOdysseyVectorSection* section = rightSideSection[i]->section;
            ::ULIS::FVec2D& sectionVector = rightSideSection[i]->GetVector();
            double dot = lastSectionVector.DotProduct( sectionVector );

            if( dot > maxDot )
            {
                rightRet = rightSideSection[i];

                maxDot = dot;
            }
        }

        if( rightRet )
        {
            return rightRet;
        }

        for( int i = 0; i < wrongSideSection.size(); i++ )
        {
            FOdysseyVectorSection* section = wrongSideSection[i]->section;
            ::ULIS::FVec2D& sectionVector = wrongSideSection[i]->GetVector();
            double dot = lastSectionVector.DotProduct( sectionVector );

            if( dot < minDot )
            {
                wrongRet = wrongSideSection[i];

                minDot = dot;
            }
        }

        if( wrongRet )
        {
            return wrongRet;
        }
    }

    return nullptr;
}

FSectionLinkInfo*
FOdysseyVectorVertex::GetOtherSectionLinkInfo( FSectionLinkInfo* iLastSectionLinkInfo )
{
    for( FSectionLinkInfo& candidateSectionInfo : mSectionLinkInfoList )
    {
        if( &candidateSectionInfo != iLastSectionLinkInfo )
        {
            return &candidateSectionInfo;
        }
    }

    return nullptr;
}

FSectionLinkInfo*
FOdysseyVectorVertex::GetCycleNextSection( FSectionLinkInfo* iLastSectionLinkInfo
                                         , double iOrientation )
{
    //std::vector<FOdysseyVectorVertex*> partnerVertexArray;
    //std::vector<FSectionLinkInfo*> candidateSectionArray;

    //partnerVertexArray.reserve( 4 );
    //candidateSectionArray.reserve( 4 );

    //GetSurroundingSections( partnerVertexArray, candidateSectionArray );

    if( mSectionLinkInfoList.size() == 2 )
    {
        return GetOtherSectionLinkInfo( iLastSectionLinkInfo );
    }

    return FindNextSectionLinkInfo( iLastSectionLinkInfo
                                  , mSectionLinkInfoList
                                  , iOrientation );
}


#ifdef unused
FSectionLinkInfo*
FOdysseyVectorVertex::GetCycleNextSection( FOdysseyVectorSection* iLastSection
                                         , uint32 iLastSectionVertexIndex
                                         , double iOrientation )
{
    std::vector<FOdysseyVectorVertex*> partnerVertexArray;
    std::vector<FSectionLinkInfo> candidateSectionArray;
    FSectionLinkInfo* rightRet = nullptr;
    FSectionLinkInfo* wrongRet = nullptr;
    uint32 sectionCount = mSectionLinkInfoList.size();

    partnerVertexArray.reserve( 4 );
    candidateSectionArray.reserve( 4 );

    GetCandidateSections( iLastSection
                        , iLastSectionVertexIndex
                        , partnerVertexArray
                        , candidateSectionArray );

/*
    if( sectionCount == 2 )
    {
        FOdysseyVectorSection* nextSection = GetOtherSection( iLastSection, false );
        uint32 nextSectionVertexIndex = this->GetIndex( nextSection );

        // note: the vector is unused in the return value
        return FSectionLinkInfo( nextSection, ::ULIS::FVec2D( 0.0f, 0.0f ), nextSectionVertexIndex );
    }
*/
//UE_LOG(LogTemp, Warning, TEXT("Hello World %d"), candidateSectionArray.size() );
    //if( candidateSectionArray.size() > 2 )
    if( candidateSectionArray.size() )
    {
        ::ULIS::FVec2D lastSectionVector = iLastSection->GetVectorFromVertex( iLastSectionVertexIndex
                                                                            , false
                                                                            , true );
        double minDot =  DBL_MAX;
        double maxDot = -DBL_MAX;
        std::vector<FSectionLinkInfo> rightSideSection;
        std::vector<FSectionLinkInfo> wrongSideSection;

        rightSideSection.reserve( sectionCount );
        wrongSideSection.reserve( sectionCount );

        for( FSectionLinkInfo& cycleSectionInfo : candidateSectionArray )
        {
            if( cycleSectionInfo.sectionVector.DistanceSquared() )
            {
                if( ( FOdysseyVector::Cross2D( -lastSectionVector, cycleSectionInfo.sectionVector ) * iOrientation >= 0.0f ) )
                {
                    rightSideSection.emplace_back( cycleSectionInfo );
                }

                if( ( FOdysseyVector::Cross2D( -lastSectionVector, cycleSectionInfo.sectionVector ) * iOrientation <= 0.0f ) )
                {
                    wrongSideSection.emplace_back( cycleSectionInfo );
                }
            }
        }

        for( int i = 0; i < rightSideSection.size(); i++ )
        {
            FOdysseyVectorSection* section = rightSideSection[i].section;
            ::ULIS::FVec2D sectionVector = rightSideSection[i].sectionVector;
            double dot = lastSectionVector.DotProduct( sectionVector );

            if( dot > maxDot )
            {
                rightRet = &rightSideSection[i];

                maxDot = dot;
            }
        }

        if( rightRet )
        {
            return *rightRet;
        }

        for( int i = 0; i < wrongSideSection.size(); i++ )
        {
            FOdysseyVectorSection* section = wrongSideSection[i].section;
            ::ULIS::FVec2D sectionVector = wrongSideSection[i].sectionVector;
            double dot = lastSectionVector.DotProduct( sectionVector );

            if( dot < minDot )
            {
                wrongRet = &wrongSideSection[i];

                minDot = dot;
            }
        }

        if( wrongRet )
        {
            return *wrongRet;
        }
    }

    return FSectionLinkInfo( nullptr, 0 );
}
#endif

void
FOdysseyVectorVertex::BuildExplorationPairs( std::vector<FExplorationPair>& oExplorationPairsArray )
{
    //std::vector<FOdysseyVectorVertex*> partnerVertexArray;
    //std::vector<FSectionLinkInfo*> candidateSectionArray;
    FSectionLinkInfo* departSectionLinkInfo = nullptr;

    //partnerVertexArray.reserve( 4 );
    //candidateSectionArray.reserve( 4 );

    //GetSurroundingSections( partnerVertexArray, candidateSectionArray );

    for( FSectionLinkInfo& returnSectionLinkInfo : mSectionLinkInfoList )
    {
        departSectionLinkInfo = FindNextSectionLinkInfo( &returnSectionLinkInfo
                                                       , mSectionLinkInfoList
                                                       , 1.0f );

        if( departSectionLinkInfo )
        {
            oExplorationPairsArray.push_back( FExplorationPair( returnSectionLinkInfo.section
                                                              , this
                                                              , departSectionLinkInfo->sectionVertexIndex
                                                              , departSectionLinkInfo->section ) );
        }
    }
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetAverageVectorOnSegmentHandle( bool iNormalize )
{
    uint32 segmentCount = GetSegmentCount();
    ::ULIS::FVec2D averageVector( 0.0f, 0.0f );

    if( segmentCount )
    {
        for( FOdysseyVectorSegment* segment : mSegmentList )
        {
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
        for( FOdysseyVectorSegment* segment : mSegmentList )
        {
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
FOdysseyVectorVertex::ArrayToSegmentArray( const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                         , std::vector<FOdysseyVectorSegment*>& oSegmentArray )
{

    if( iVertexArray.size() )
    {
        oSegmentArray.reserve( iVertexArray.size() );

        for( int i = 0; i < iVertexArray.size(); i++ )
        {
            FOdysseyVectorVertex* vertex = iVertexArray[i];

            for( FOdysseyVectorSegment* segment : vertex->GetSegmentList() )
            {
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
        for( FOdysseyVectorSegment* segment : mSegmentList )
        {
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

uint32
FOdysseyVectorVertex::GetIndex( FOdysseyVectorSegment* iSegment )
{
    return ( this == iSegment->GetVertex(0) ) ? 0 : 1;
}

uint32
FOdysseyVectorVertex::GetIndex( FOdysseyVectorSection* iSection )
{
    return ( this == iSection->GetVertex(0) ) ? 0 : 1;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetNearestSegment()
{
    return mNearestSegment;
}

::ULIS::FVec2D
FOdysseyVectorVertex::GetNearestSegmentIntersectionCoords()
{
    return mNearestSegmentIntersectionCoords;
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
FOdysseyVectorVertex::ResetNearestSegment()
{
    mNearestSegment = nullptr;
    mDistanceToNearestSegment = DBL_MAX;
    mNearestSegmentT = 0.0f;
    mNearestSegmentIntersectionCoords = 0.0f;
    //mNearestVertex = nullptr;
}

void
FOdysseyVectorVertex::SetNearestSegment( FOdysseyVectorSegment* iNearestSegment
                                       , double iDistanceToNearestSegment
                                       , double iNearestSegmentT
                                       , const ::ULIS::FVec2D& iNearestSegmentIntersectionCoords )
{
    mNearestSegment = iNearestSegment;
    mDistanceToNearestSegment = iDistanceToNearestSegment;
    mNearestSegmentT = iNearestSegmentT;
    mNearestSegmentIntersectionCoords = iNearestSegmentIntersectionCoords;
    //mNearestVertex = nullptr;
}

void
FOdysseyVectorVertex::ResetNearestVertex()
{
    mDistanceToNearestVertex = DBL_MAX;
    mNearestVertex = nullptr;
}

void
FOdysseyVectorVertex::SetNearestVertex( FOdysseyVectorVertex* iNearestVertex
                                      , double iDistanceToNearestVertex )
{
    mDistanceToNearestVertex = iDistanceToNearestVertex;
    mNearestVertex = iNearestVertex;
}

FOdysseyVectorVertex*
FOdysseyVectorVertex::GetNearestVertex()
{
    return mNearestVertex;
}

double
FOdysseyVectorVertex::GetDistanceToNearestVertex()
{
    return mDistanceToNearestVertex;
}

void
FOdysseyVectorVertex::InvalidateSegments()
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        segment->Invalidate();
    }
}

double
FOdysseyVectorVertex::GetRadius()
{
    return mRadius;
}

void
FOdysseyVectorVertex::SetRadius( double iRadius )
{
    if( ( mFlags & LOCKED ) == 0 )
    {
        // quickfix. sometimes intbetweenerTag functions pass a negative radius
        // when the shape is committed but I don't know why.
        mRadius = fabs( iRadius );

        Invalidate();
    }
}

void
FOdysseyVectorVertex::Update( FOdysseyVectorSegment* iPrevSegment, FOdysseyVectorSegment* iSegment )
{
    // update joint
    MakeJoint( iPrevSegment, iSegment );

    mFlags &= (~INVALIDATED);
}

void
FOdysseyVectorVertex::Invalidate()
{
    mFlags |= INVALIDATED;

    InvalidateSegments();
}

bool
FOdysseyVectorVertex::IsInvalidated()
{
    return ( mFlags & INVALIDATED ) ? true : false;
}

bool
FOdysseyVectorVertex::IsLocked()
{
    return ( mFlags & LOCKED ) ? true : false;
}

void
FOdysseyVectorVertex::SetLocked( bool iIsLocked )
{
    if( iIsLocked == true )
    {
        mFlags |= LOCKED;
    }
    else
    {
        mFlags &= (~LOCKED);
    }
}

void
FOdysseyVectorVertex::SetCoords( double iX, double iY )
{
    if( ( mFlags & LOCKED ) == 0 )
    {
        FOdysseyVectorPoint::SetCoords( iX, iY );

        // commented-out: untouched former bbox is needed to update the correct region of the screen
        //mJoint.ResetBBox();

        Invalidate();
    }


}

void
FOdysseyVectorVertex::SetOwner( FOdysseyVectorObject* iOwner )
{
    mOwner = iOwner;
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
    return mSectionLinkInfoList.size();
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorVertex::GetSegmentList()
{
    return mSegmentList;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetLastSegment()
{
    return mSegmentList.size() ? mSegmentList.back() : nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetFirstSegment()
{
    return mSegmentList.size() ? mSegmentList.front() : nullptr;
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetFirstSection()
{
    return mSectionLinkInfoList.size() ? mSectionLinkInfoList.front().section : nullptr;
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
FOdysseyVectorVertex::GetHandlePosition( ::ULIS::FVec2D iHandlePosition[2] )
{
    FOdysseyVectorSegment* segment = GetFirstSegment();

    iHandlePosition[0] = iHandlePosition[1] = mCoords;

    if( segment && segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
        FOdysseyVectorOffsetCurveCubic* offsetCurve[2] = { cubicSegment->GetOffsetCurve( 0 )
                                                         , cubicSegment->GetOffsetCurve( 1 ) };
        std::vector<FOdysseyVectorBezierFragment>& offsetCurve0BezierFragmentArray = offsetCurve[0]->GetBezierFragmentArray();
        std::vector<FOdysseyVectorBezierFragment>& offsetCurve1BezierFragmentArray = offsetCurve[1]->GetBezierFragmentArray();

        if( offsetCurve0BezierFragmentArray.size() && offsetCurve1BezierFragmentArray.size() )
        {
            iHandlePosition[0] = ( segment->GetVertex(0) == this ) ? offsetCurve0BezierFragmentArray.front().bezier[0]
                                                                   : offsetCurve0BezierFragmentArray.back().bezier[3];
            iHandlePosition[1] = ( segment->GetVertex(0) == this ) ? offsetCurve1BezierFragmentArray.front().bezier[0]
                                                                   : offsetCurve1BezierFragmentArray.back().bezier[3];
        }
    }
}

// TODO: rename as SetSegmentAligned
void
FOdysseyVectorVertex::SetHandleAligned( bool iHandleAligned )
{
    if( iHandleAligned == true )
    {
        FOdysseyVectorSegment* firstSegment = GetFirstSegment();

        mFlags |= HANDLE_ALIGNED;

        if( firstSegment )
        {
            FOdysseyVectorHandleSegment* handle = firstSegment->GetHandle( this );

            // force alignment of handles
            handle->Set( handle->GetCoords() );
        }
    }
    else
    {
        mFlags &= (~HANDLE_ALIGNED);
    }

    Invalidate();
}

// TODO: rename as IsSegmentAligned
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

uint32
FOdysseyVectorVertex::GetFlags()
{
    return mFlags;
}

void
FOdysseyVectorVertex::AddSection( FOdysseyVectorSection* iSection
                                , uint32 iSectionVertexIndex )
{
    mSectionLinkInfoList.emplace_back( iSection, iSectionVertexIndex );
}

void
FOdysseyVectorVertex::RemoveSection( FOdysseyVectorSection* iSection
                                   , uint32 iSectionVertexIndex )
{
    mSectionLinkInfoList.remove_if( [ iSection
                                    , iSectionVertexIndex ]( FSectionLinkInfo& sectionLinkInfo )
                                    {
                                        return ( ( sectionLinkInfo.section            == iSection            )
                                              && ( sectionLinkInfo.sectionVertexIndex == iSectionVertexIndex ) );
                                    } );
}

::ULIS::FRectD
FOdysseyVectorVertex::GetBoundingBox( bool iWorld )
{
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    if( iWorld )
    {
        BLPoint pt = mOwner->GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );

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

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
        ::ULIS::FRectD rect = segment->GetBoundingBox( iWorld );

        bbox = bbox | rect;
    }

    return bbox;
}

void
FOdysseyVectorVertex::GetSectionLinkInfo( std::vector<FSectionLinkInfo>& oSectionLinkInfoArray )
{
    oSectionLinkInfoArray.reserve( mSectionLinkInfoList.size() );

    for( FSectionLinkInfo& sectionLinkInfo : mSectionLinkInfoList )
    {
        oSectionLinkInfoArray.emplace_back( sectionLinkInfo );
    }
}

std::list<FSectionLinkInfo>&
FOdysseyVectorVertex::GetSectionLinkInfoList()
{
    return mSectionLinkInfoList;
}

FOdysseyVectorSection*
FOdysseyVectorVertex::GetSection( FOdysseyVectorSegment* iSegment )
{
    for( FSectionLinkInfo& sectionLinkInfo : mSectionLinkInfoList )
    {
        if( sectionLinkInfo.section->GetSegment() == iSegment )
        {
            return sectionLinkInfo.section;
        }
    }

    return nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetSegment( FOdysseyVectorVertex* iOtherVertex )
{
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
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
    for( FSectionLinkInfo& otherSectionLinkInfo : mSectionLinkInfoList )
    {
        if( otherSectionLinkInfo.section != iSection )
        {
            if( iSameSegment == true )
            {
                if ( otherSectionLinkInfo.section->GetSegment() == iSection->GetSegment() )
                {
                    return otherSectionLinkInfo.section;
                }
            }
            else
            {
                return otherSectionLinkInfo.section;
            }
        }
    }

    return nullptr;
}

FOdysseyVectorSegment*
FOdysseyVectorVertex::GetOtherSegment( FOdysseyVectorSegment* iSegment )
{
    for( FOdysseyVectorSegment* otherSegment : mSegmentList )
    {
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
FOdysseyVectorVertex::DrawJoint( BLContext* iBLContext
                               , double iCombinedOpacity
                               , uint64 iDrawingFlags )
{
    if( mSegmentList.size() == 2 )
    {
        if( IsHandleAligned() == true )
        {
            BLMatrix2D& worldMatrix = mOwner->GetWorldMatrix();
            ::ULIS::FVec2D localHandlePosition[2];
            BLPoint worldHandlePosition[2];

            GetHandlePosition( localHandlePosition );

            worldHandlePosition[0] = worldMatrix.mapPoint( localHandlePosition[0].x, localHandlePosition[0].y );
            worldHandlePosition[1] = worldMatrix.mapPoint( localHandlePosition[1].x, localHandlePosition[1].y );

            iBLContext->save();
            iBLContext->resetMatrix();
            iBLContext->strokeLine( worldHandlePosition[0], worldHandlePosition[1] );
            iBLContext->restore();
        }
        else
        {
            mJoint.Draw( iBLContext
                       , mJoint.GetTextureStartU()
                       , mJoint.GetTextureEndU()
                       , iCombinedOpacity
                       , iDrawingFlags );
        }
    }
}

void
FOdysseyVectorVertex::MakeJoint( FOdysseyVectorSegment* iPrevSegment
                               , FOdysseyVectorSegment* iNextSegment )
{
    mJoint.Make( iPrevSegment, iNextSegment );
}
