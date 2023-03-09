#include "OdysseyVectorHandleSegment.h"

FOdysseyVectorHandleSegment::~FOdysseyVectorHandleSegment()
{
}

FOdysseyVectorHandleSegment::FOdysseyVectorHandleSegment()
    : mParentSegment ( nullptr )
{
}

void
FOdysseyVectorHandleSegment::Init( FOdysseyVectorSegment* iParentSegment, double iX, double iY )
{
    mParentSegment = iParentSegment;

    FOdysseyVectorHandle::Init( iX, iY );
}

//static
FOdysseyVectorHandleSegment*
FOdysseyVectorHandleSegment::New( FOdysseyVectorSegment* iParentSegment, double iX, double iY )
{
    FOdysseyVectorHandleSegment* handleSegment = new FOdysseyVectorHandleSegment();

    handleSegment->Init( iParentSegment, iX, iY );

    return handleSegment;
}

FOdysseyVectorSegment*
FOdysseyVectorHandleSegment::GetParent()
{
    return mParentSegment;
}

void 
FOdysseyVectorHandleSegment::SetX( double iX )
{
    mCoords.x  = iX;

    mParentSegment->Invalidate();
}

void 
FOdysseyVectorHandleSegment::SetY( double iY )
{
    mCoords.y = iY;

    mParentSegment->Invalidate();
}

void 
FOdysseyVectorHandleSegment::Set( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;

    mParentSegment->Invalidate();
}
