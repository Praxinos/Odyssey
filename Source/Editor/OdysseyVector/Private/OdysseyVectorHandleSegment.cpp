#include "OdysseyVectorHandleSegment.h"

FOdysseyVectorHandleSegment::~FOdysseyVectorHandleSegment()
{
}

FOdysseyVectorHandleSegment::FOdysseyVectorHandleSegment( FOdysseyVectorSegment& iParentSegment, double iX, double iY )
    : FOdysseyVectorHandle( iX, iY )
   , mParentSegment ( iParentSegment )
{

}

uint32
FOdysseyVectorHandleSegment::GetType()
{
    return FOdysseyVectorPoint::POINT_TYPE_HANDLE_SEGMENT;
}

FOdysseyVectorSegment&
FOdysseyVectorHandleSegment::GetParent()
{
    return mParentSegment;
}

void 
FOdysseyVectorHandleSegment::SetX( double iX )
{
    mCoords.x  = iX;

    mParentSegment.Invalidate();
}

void 
FOdysseyVectorHandleSegment::SetY( double iY )
{
    mCoords.y = iY;

    mParentSegment.Invalidate();
}

void 
FOdysseyVectorHandleSegment::Set( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;

    mParentSegment.Invalidate();
}
