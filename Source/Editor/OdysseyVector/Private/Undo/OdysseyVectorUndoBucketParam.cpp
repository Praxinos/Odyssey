#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "OdysseyVectorEngine.h"

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
FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorGroupPaint* iScene
                                                            , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo( iScene )
{
    mBucketSnapshotArray.push_back( FSnapshotBucket( iBucket, FSnapshotFlags::Point::Bucket::PARAM ));
}

// Backup bucket params in the constructor
FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorGroupPaint* iScene
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
        mBucketSnapshotArray.push_back( FSnapshotBucket( iBucketArray[i], FSnapshotFlags::Point::Bucket::PARAM ));
    }
}

void
FOdysseyVectorUndoBucketParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
    {
        mBucketSnapshotArray[i].Restore();
    }

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
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoBucketParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
    {
        mBucketSnapshotArray[i].Restore();
    }

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
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

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
