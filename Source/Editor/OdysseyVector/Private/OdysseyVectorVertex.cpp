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

        oXMin = oXMax = firstVertexCoords.x;
        oYMin = oYMax = firstVertexCoords.y;

        for( FOdysseyVectorVertex* vertex : iVertexList )
        {
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

        for( FOdysseyVectorSection* section : mSectionList )
        {
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
    for( FOdysseyVectorSection* returnSection : mSectionList )
    {
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
    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
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

// TODO: rename as AlignSegments
void
FOdysseyVectorVertex::AlignHandles()
{
    if( mSegmentList.size() )
    {
        AlignHandles( mSegmentList.front()->GetHandle( this ) );
    }
}

// TODO: rename as AlignSegments
void
FOdysseyVectorVertex::AlignHandles( FOdysseyVectorHandleSegment* iHandle )
{
    ::ULIS::FVec2D handleVector = iHandle->GetCoords() - mCoords;
    FOdysseyVectorSegment* segment = iHandle->GetOwner();
    FOdysseyVectorSegment* otherSegment = GetOtherSegment( segment );

    if( otherSegment )
    {
        FOdysseyVectorHandleSegment* otherHandle = otherSegment->GetHandle( this );
        ::ULIS::FVec2D otherHandleVector = otherHandle->GetCoords() - mCoords;

        // if vectors are already aligned, their cross product equals 0
        if( FOdysseyVector::Cross2D( otherHandleVector, handleVector ) )
        {
            handleVector.Normalize();

            otherHandle->Set( GetCoords() - ( otherHandleVector.Distance() * handleVector ) );
        }
    }

    SetHandleAligned( true );
}

// TODO: rename as SetSegmentAligned
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
FOdysseyVectorVertex::AddSection( FOdysseyVectorSection* iSection )
{
    mSectionList.push_back( iSection );
}

void
FOdysseyVectorVertex::RemoveSection( FOdysseyVectorSection* iSection )
{
    mSectionList.remove( iSection );
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

    for( FOdysseyVectorSegment* segment : mSegmentList )
    {
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
    for( FOdysseyVectorSection* section : mSectionList )
    {
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
    for( FOdysseyVectorSection* otherSection : mSectionList )
    {
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
FOdysseyVectorVertex::DrawJoint( BLContext* iBLContext, uint64 iDrawingFlags )
{
    if( mSegmentList.size() == 2 )
    {
        if( IsHandleAligned() == true )
        {
            BLMatrix2D& worldMatrix = mPath->GetWorldMatrix();
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
            mJoint.Draw( iBLContext, iDrawingFlags );
        }
    }
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
