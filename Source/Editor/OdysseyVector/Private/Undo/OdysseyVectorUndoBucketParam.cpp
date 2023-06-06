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
                                                            , FOdysseyVectorObject& iBucketParent
                                                            , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo( iScene )
    , mBucketSave( iBucketParent )
    , mBucket( iBucket )
{
    mBucket->Copy( &mBucketSave );
}

void
FOdysseyVectorUndoBucketParam::Apply( UObject* iIgnored )
{
    FOdysseyVectorBucket tmpBucketSave( mBucketSave.GetParent() );

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mBucket->Copy( &tmpBucketSave );
    mBucketSave.Copy( mBucket );
    tmpBucketSave.Copy( &mBucketSave );

    mBucket->Invalidate();

    // update invalidated objects
    mScene->Update(0);

    FOdysseyVectorEngine::ResetHUD( mScene );
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoBucketParam::Revert( UObject* iIgnored )
{
    FOdysseyVectorBucket tmpBucketSave( mBucketSave.GetParent() );

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mBucket->Copy( &tmpBucketSave );
    mBucketSave.Copy( mBucket );
    tmpBucketSave.Copy( &mBucketSave );

    mBucket->Invalidate();

    // update invalidated objects
    mScene->Update(0);

    FOdysseyVectorEngine::ResetHUD( mScene );
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketParam::ToString() const
{
    return FString("FOdysseyVectorUndoBucketParam");
}
