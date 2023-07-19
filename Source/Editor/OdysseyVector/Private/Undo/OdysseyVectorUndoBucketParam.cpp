#include "Undo/OdysseyVectorUndoBucketParam.h"

FOdysseyVectorUndoBucketParam::~FOdysseyVectorUndoBucketParam()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorScene* iScene
                                                            , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo( iScene )
    , mBucketSave( iBucket->GetOwner(), 0.0f, 0.0f, false )
    , mBucket( iBucket )
{
    mBucket->Copy( &mBucketSave );
}

void
FOdysseyVectorUndoBucketParam::Apply( UObject* iIgnored )
{
    FOdysseyVectorBucket tmpBucketSave( mBucketSave.GetOwner(), 0.0f, 0.0f, false );

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mBucket->Copy( &tmpBucketSave );
    mBucketSave.Copy( mBucket );
    tmpBucketSave.Copy( &mBucketSave );

    mBucket->Invalidate();

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoBucketParam::Revert( UObject* iIgnored )
{
    FOdysseyVectorBucket tmpBucketSave( mBucketSave.GetOwner(), 0.0f, 0.0f, false );

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mBucket->Copy( &tmpBucketSave );
    mBucketSave.Copy( mBucket );
    tmpBucketSave.Copy( &mBucketSave );

    mBucket->Invalidate();

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketParam::ToString() const
{
    return FString("FOdysseyVectorUndoBucketParam");
}
