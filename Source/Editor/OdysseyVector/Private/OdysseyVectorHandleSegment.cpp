#include "OdysseyVectorHandleSegment.h"

UOdysseyVectorHandleSegment::~UOdysseyVectorHandleSegment()
{
}

UOdysseyVectorHandleSegment::UOdysseyVectorHandleSegment()
    : mParentSegment ( nullptr )
{
}

void
UOdysseyVectorHandleSegment::Init( UOdysseyVectorSegment* iParentSegment, double iX, double iY )
{
    mParentSegment = iParentSegment;

    UOdysseyVectorHandle::Init( iX, iY );
}

//static
UOdysseyVectorHandleSegment*
UOdysseyVectorHandleSegment::New( UOdysseyVectorSegment* iParentSegment, double iX, double iY )
{
    UOdysseyVectorHandleSegment* handleSegment = NewObject<UOdysseyVectorHandleSegment>();

    handleSegment->Init( iParentSegment, iX, iY );

    return handleSegment;
}

UOdysseyVectorSegment*
UOdysseyVectorHandleSegment::GetParent()
{
    return mParentSegment;
}

void 
UOdysseyVectorHandleSegment::SetX( double iX )
{
    mCoords.x  = iX;

    mParentSegment->Invalidate();
}

void 
UOdysseyVectorHandleSegment::SetY( double iY )
{
    mCoords.y = iY;

    mParentSegment->Invalidate();
}

void 
UOdysseyVectorHandleSegment::Set( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;

    mParentSegment->Invalidate();
}
