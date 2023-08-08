#include "Undo/OdysseyVectorUndoBucketRemove.h"

FOdysseyVectorUndoBucketRemove::~FOdysseyVectorUndoBucketRemove()
{
    if( mApplied )
    {
        delete mBucket;
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoBucketRemove::FOdysseyVectorUndoBucketRemove( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo( iScene )
    , mBucket( iBucket )
{
}

void
FOdysseyVectorUndoBucketRemove::Apply( UObject* iIgnored )
{
    FOdysseyVectorObject* ownerObject = mBucket->GetOwner();

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    if( ownerObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( mBucket );
        paintGroup->Colorize();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoBucketRemove::Revert( UObject* iIgnored )
{
    FOdysseyVectorObject* ownerObject = mBucket->GetOwner();

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    if( ownerObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->AddBucket( mBucket );
        paintGroup->Colorize();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketRemove::ToString() const
{
    return FString("FOdysseyVectorUndoBucketRemove");
}
