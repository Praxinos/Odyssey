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
{
    mBucketSaveArray.emplace_back( iBucket->GetOwner(), 0.0f, 0.0f, false );
    mBucketArray.push_back( iBucket );
}

FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorScene* iScene
                                                            , std::vector<FOdysseyVectorBucket*>& iBucketArray )
    : FOdysseyVectorUndo( iScene )
{
    mBucketArray = iBucketArray;

    for( int i = 0; i < mBucketArray.size(); i++ )
    {
        mBucketSaveArray.emplace_back( mBucketArray[i]->GetOwner(), 0.0f, 0.0f, false );

        mBucketArray[i]->Copy( &mBucketSaveArray[i] );
    }
}

void
FOdysseyVectorUndoBucketParam::Swap()
{
    for( int i = 0; i < mBucketArray.size(); i++ )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        FOdysseyVectorBucket tmpBucketSave( mBucketSaveArray[i].GetOwner(), 0.0f, 0.0f, false );

        // save data to tmp
        mBucketArray[i]->Copy( &tmpBucketSave );
        // restore bucket data
        mBucketSaveArray[i].Copy( mBucketArray[i] );
        // swap data from tmp
        tmpBucketSave.Copy( &mBucketSaveArray[i] );

        mBucketArray[i]->Invalidate();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoBucketParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    Swap();
}

void
FOdysseyVectorUndoBucketParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    Swap();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketParam::ToString() const
{
    return FString("FOdysseyVectorUndoBucketParam");
}
