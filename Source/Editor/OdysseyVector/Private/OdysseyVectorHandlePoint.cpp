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
UOdysseyVectorHandlePoint::Init( FOdysseyVectorPoint* iParentPoint )
{
    mParentPoint = iParentPoint;
}

//static
UOdysseyVectorHandlePoint*
UOdysseyVectorHandlePoint::New( FOdysseyVectorPoint* iParentPoint )
{
    UOdysseyVectorHandlePoint* handlePoint = NewObject<UOdysseyVectorHandlePoint>();

    handlePoint->Init( iParentPoint );

    return handlePoint;
}

FOdysseyVectorPoint*
UOdysseyVectorHandlePoint::GetParent()
{
    return mParentPoint;
}
