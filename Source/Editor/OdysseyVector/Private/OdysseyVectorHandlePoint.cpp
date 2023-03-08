#include "OdysseyVectorHandlePoint.h"

FOdysseyVectorHandlePoint::~FOdysseyVectorHandlePoint()
{
}

FOdysseyVectorHandlePoint::FOdysseyVectorHandlePoint()
    : FOdysseyVectorHandle()
    , mParentPoint ( nullptr )
{

}

void
FOdysseyVectorHandlePoint::Init( FOdysseyVectorPoint* iParentPoint )
{
    mParentPoint = iParentPoint;
}

//static
FOdysseyVectorHandlePoint*
FOdysseyVectorHandlePoint::New( FOdysseyVectorPoint* iParentPoint )
{
    FOdysseyVectorHandlePoint* handlePoint = NewObject<FOdysseyVectorHandlePoint>();

    handlePoint->Init( iParentPoint );

    return handlePoint;
}

FOdysseyVectorPoint*
FOdysseyVectorHandlePoint::GetParent()
{
    return mParentPoint;
}
