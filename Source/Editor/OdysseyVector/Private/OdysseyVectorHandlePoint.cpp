#include "OdysseyVectorHandlePoint.h"

FOdysseyVectorHandlePoint::~FOdysseyVectorHandlePoint()
{
}

FOdysseyVectorHandlePoint::FOdysseyVectorHandlePoint( FOdysseyVectorPoint& iParentPoint )
    : FOdysseyVectorHandle()
    , mParentPoint ( iParentPoint )
{

}

FOdysseyVectorPoint&
FOdysseyVectorHandlePoint::GetParent()
{
    return mParentPoint;
}

uint32
FOdysseyVectorHandlePoint::GetType()
{
    return FOdysseyVectorPoint::POINT_TYPE_HANDLE_POINT;
}
