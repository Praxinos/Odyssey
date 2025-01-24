// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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

void
FOdysseyVectorTag::LockDrawing()
{
    //UE_LOG(LogTemp, Warning, TEXT("LockDrawing"));

    mDrawingMutex.lock();
}

void
FOdysseyVectorTag::UnlockDrawing()
{
    mDrawingMutex.unlock();

    //UE_LOG(LogTemp, Warning, TEXT("UnlockDrawing"));
}
