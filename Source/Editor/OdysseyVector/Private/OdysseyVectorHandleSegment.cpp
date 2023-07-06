#include "OdysseyVectorHandleSegment.h"

FOdysseyVectorHandleSegment::~FOdysseyVectorHandleSegment()
{
}

FOdysseyVectorHandleSegment::FOdysseyVectorHandleSegment( FOdysseyVectorSegment* iOwnerSegment, double iX, double iY )
    : FOdysseyVectorPoint( iX, iY, 0.0f )
    , mOwnerSegment ( iOwnerSegment )
{
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
