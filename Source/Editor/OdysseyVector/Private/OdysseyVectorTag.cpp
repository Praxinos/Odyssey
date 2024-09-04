#include "OdysseyVectorTag.h"

FOdysseyVectorTag::~FOdysseyVectorTag()
{
}

FOdysseyVectorTag::FOdysseyVectorTag( FOdysseyVectorObject* iOwnerObject )
    : mOwner( iOwnerObject )
    , mFlags( 0 )
{
}

FOdysseyVectorObject*
FOdysseyVectorTag::GetOwner()
{
    return mOwner;
}
