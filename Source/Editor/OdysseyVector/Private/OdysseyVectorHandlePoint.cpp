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

UOdysseyVectorPoint*
UOdysseyVectorHandlePoint::GetParent()
{
    return mParentPoint;
}
