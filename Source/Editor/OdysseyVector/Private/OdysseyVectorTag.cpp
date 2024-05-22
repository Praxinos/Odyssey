#include "OdysseyVectorTag.h"

FOdysseyVectorTag::~FOdysseyVectorTag()
{
}

FOdysseyVectorTag::FOdysseyVectorTag( FOdysseyVectorObject* iOwnerObject )
    : mOwner( iOwnerObject )
{
}

FOdysseyVectorObject*
FOdysseyVectorTag::GetOwner()
{
    return mOwner;
}
