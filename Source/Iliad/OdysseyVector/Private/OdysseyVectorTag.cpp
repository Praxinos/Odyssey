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

void
FOdysseyVectorTag::LockDrawing()
{
    mDrawingMutex.lock();
}

void
FOdysseyVectorTag::UnlockDrawing()
{
    mDrawingMutex.unlock();
}
