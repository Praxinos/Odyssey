#include "View/OdysseyVectorViewObject.h"

UOdysseyVectorViewObject::~UOdysseyVectorViewObject()
{
}

UOdysseyVectorViewObject::UOdysseyVectorViewObject()
{
}

void 
UOdysseyVectorViewObject::Update( FOdysseyVectorObject* iVectorObject )
{
    mObjectParam = iVectorObject->mObjectParam;
}
