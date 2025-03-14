// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorSegmentCubicGap.h"

// static
void
FOdysseyVectorHandleSegment::ArrayToVertexArray( const std::vector<FOdysseyVectorHandleSegment*>& iSegmentHandleArray
                                               , std::vector<FOdysseyVectorVertex*>& oVertexArray )
{
    for( FOdysseyVectorHandleSegment* segmentHandle : iSegmentHandleArray )
    {
        FOdysseyVectorSegment* segment = segmentHandle->GetOwner();

        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            oVertexArray.push_back( segmentHandle->GetAttachedVertex() );
        }
    }
}

FOdysseyVectorHandleSegment::~FOdysseyVectorHandleSegment()
{
}

FOdysseyVectorHandleSegment::FOdysseyVectorHandleSegment( FOdysseyVectorSegment* iOwnerSegment
                                                        , FOdysseyVectorVertex* iAttachedVertex
                                                        , double iX
                                                        , double iY )
    : FOdysseyVectorPoint( iX, iY )
    , mOwnerSegment ( iOwnerSegment )
    , mAttachedVertex( iAttachedVertex )
{
}

void
FOdysseyVectorHandleSegment::SetID( uint32 iHandleID )
{
    mID = iHandleID;
}

uint32
FOdysseyVectorHandleSegment::GetID()
{
    return mID;
}

FOdysseyVectorVertex*
FOdysseyVectorHandleSegment::GetAttachedVertex()
{
    return mAttachedVertex;
}

FOdysseyVectorSegment*
FOdysseyVectorHandleSegment::GetOwner()
{
    return mOwnerSegment;
}

void
FOdysseyVectorHandleSegment::SetCoordsSilent( ::ULIS::FVec2D& iCoords )
{
    mCoords = iCoords;
}

void
FOdysseyVectorHandleSegment::SetCoords( double iX, double iY )
{
    // invalidate the attached vertex in order to update the joint
    mAttachedVertex->Invalidate();

    if( mAttachedVertex->IsLocked() == false )
    {
        FOdysseyVectorPoint::SetCoords( iX, iY );

        if( mOwnerSegment )
        {
            mOwnerSegment->Invalidate();
        }

        if( mAttachedVertex->IsHandleAligned() )
        {
            ::ULIS::FVec2D handleVector = mCoords - mAttachedVertex->GetCoords();
            FOdysseyVectorSegment* otherSegment = mAttachedVertex->GetOtherSegment( mOwnerSegment );

            if( otherSegment )
            {
                FOdysseyVectorHandleSegment* otherHandle = otherSegment->GetHandle( mAttachedVertex );
                ::ULIS::FVec2D otherHandleVector = otherHandle->GetCoords() - mAttachedVertex->GetCoords();

                if( handleVector.DistanceSquared() )
                {
                    handleVector.Normalize();

                    ::ULIS::FVec2D alignedCoords = mAttachedVertex->GetCoords() - ( otherHandleVector.Distance() * handleVector );

                    otherHandle->FOdysseyVectorPoint::SetCoords( alignedCoords.x, alignedCoords.y );

                    otherSegment->Invalidate();
                }
            }
        }
    }
}
