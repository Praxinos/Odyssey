#include "OdysseyVectorHandleSegment.h"

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
