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

            oVertexArray.push_back( cubicSegment->GetVertex( segmentHandle->GetHandleID() ) );
        }
    }
}

FOdysseyVectorHandleSegment::~FOdysseyVectorHandleSegment()
{
}

FOdysseyVectorHandleSegment::FOdysseyVectorHandleSegment( FOdysseyVectorSegment* iOwnerSegment, uint32 iHandleID, double iX, double iY )
    : FOdysseyVectorPoint( iX, iY, 0.0f )
    , mOwnerSegment ( iOwnerSegment )
    , mHandleID(iHandleID)
{
}

uint32
FOdysseyVectorHandleSegment::GetHandleID()
{
    return mHandleID;
}

FOdysseyVectorSegment*
FOdysseyVectorHandleSegment::GetOwner()
{
    return mOwnerSegment;
}

void 
FOdysseyVectorHandleSegment::SetCoords( double iX, double iY, double iRadius )
{
    FOdysseyVectorPoint::SetCoords( iX, iY, iRadius );

    if( mOwnerSegment )
    {
        mOwnerSegment->Invalidate();
    }
}

