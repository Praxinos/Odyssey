#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorTag::~FOdysseyVectorTag()
{
}

FOdysseyVectorTag::FOdysseyVectorTag( FOdysseyVectorObject* iOwnerObject )
    : mOwner( iOwnerObject )
    , mFlags( 0 )
    , bShared( false )
{
}

FOdysseyVectorObject*
FOdysseyVectorTag::GetOwner()
{
    return mOwner;
}

void
FOdysseyVectorTag::Share( FOdysseyVectorSharedEnv* iSharedEnv )
{
    iSharedEnv->AddSharedTag( this );

    bShared = true;
}

void
FOdysseyVectorTag::Unshare( FOdysseyVectorSharedEnv* iSharedEnv )
{
    iSharedEnv->RemoveSharedTag( this );

    bShared = false;
}