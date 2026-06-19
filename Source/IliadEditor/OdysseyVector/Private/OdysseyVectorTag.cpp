// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
FOdysseyVectorTag::Share( FOdysseyVectorLayer* iSharedEnv )
{
    iSharedEnv->AddSharedTag( this );

    bShared = true;
}

void
FOdysseyVectorTag::Unshare( FOdysseyVectorLayer* iSharedEnv )
{
    iSharedEnv->RemoveSharedTag( this );

    bShared = false;
}

void
FOdysseyVectorTag::ObjectAdded()
{
}

void
FOdysseyVectorTag::ObjectRemoved()
{
}

void
FOdysseyVectorTag::Added()
{
}

void
FOdysseyVectorTag::Removed()
{
}
