#include "Undo/OdysseyVectorUndoBucketParam.h"

FOdysseyVectorUndoBucketParam::~FOdysseyVectorUndoBucketParam()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        for( int i = 0; i < mAddedBucketArray.size(); i++ )
        {
            delete mAddedBucketArray[i];
        }
    }
}

// Backup bucket params in the constructor
FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorScene* iScene
                                                            , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo( iScene )
{
    mBucketSnapshotArray.emplace_back( iBucket, FSnapshotBucket::SNAPSHOT_PARAM );
}

// Backup bucket params in the constructor
FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorScene* iScene
                                                            , std::vector<FOdysseyVectorBucket*>& iAddedBucketArray
                                                            , std::vector<FOdysseyVectorBucket*>& iBucketArray )
    : FOdysseyVectorUndo( iScene )
{
    mAddedBucketArray = iAddedBucketArray;

    mBucketSnapshotArray.reserve( iBucketArray.size() );

    for( int i = 0; i < iBucketArray.size(); i++ )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mBucketSnapshotArray.emplace_back( iBucketArray[i], FSnapshotBucket::SNAPSHOT_PARAM );
    }
}

void
FOdysseyVectorUndoBucketParam::Swap()
{
    for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
    {
        mBucketSnapshotArray[i].Restore();
    }
}

void
FOdysseyVectorUndoBucketParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    Swap();

    for( int i = 0; i < mAddedBucketArray.size(); i++ )
    {
        FOdysseyVectorObject* ownerObject = mAddedBucketArray[i]->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->AddBucket( mAddedBucketArray[i] );
        }
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoBucketParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    Swap();

    for( int i = 0; i < mAddedBucketArray.size(); i++ )
    {
        FOdysseyVectorObject* ownerObject = mAddedBucketArray[i]->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->RemoveBucket( mAddedBucketArray[i] );
        }
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

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
