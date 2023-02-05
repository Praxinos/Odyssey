#include "OdysseyVectorHandlePoint.h"

UOdysseyVectorHandlePoint::~UOdysseyVectorHandlePoint()
{
}

UOdysseyVectorHandlePoint::UOdysseyVectorHandlePoint()
    : UOdysseyVectorHandle()
    , mParentPoint ( nullptr )
{

}

void
UOdysseyVectorHandlePoint::Init( UOdysseyVectorPoint* iParentPoint )
{
    mParentPoint = iParentPoint;
}

//static
UOdysseyVectorHandlePoint*
UOdysseyVectorHandlePoint::New( UOdysseyVectorPoint* iParentPoint )
{
    UOdysseyVectorHandlePoint* handlePoint = NewObject<UOdysseyVectorHandlePoint>();

    handlePoint->Init( iParentPoint );

    return handlePoint;
}

UOdysseyVectorPoint*
UOdysseyVectorHandlePoint::GetParent()
{
    return mParentPoint;
}
 