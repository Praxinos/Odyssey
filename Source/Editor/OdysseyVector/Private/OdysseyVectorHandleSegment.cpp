#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"

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

            oVertexArray.push_back( cubicSegment->GetVertex( segmentHandle->GetID() ) );
        }
    }
}

FOdysseyVectorHandleSegment::~FOdysseyVectorHandleSegment()
{
}

FOdysseyVectorHandleSegment::FOdysseyVectorHandleSegment( FOdysseyVectorSegment* iOwnerSegment, uint32 iHandleID, double iX, double iY )
    : FOdysseyVectorPoint( iX, iY )
    , mOwnerSegment ( iOwnerSegment )
    , mID(iHandleID)
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

FOdysseyVectorSegment*
FOdysseyVectorHandleSegment::GetOwner()
{
    return mOwnerSegment;
}

void 
FOdysseyVectorHandleSegment::SetCoords( double iX, double iY )
{
    if( mOwnerSegment->GetVertex( mID )->IsLocked() == false )
    {
        FOdysseyVectorPoint::SetCoords( iX, iY );

        if( mOwnerSegment )
        {
            mOwnerSegment->Invalidate();
        }
    }
}

