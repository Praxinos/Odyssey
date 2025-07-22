// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

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
                                                            , FOdysseyVectorBucket* iBucket
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mBucketSnapshotBuffer.emplace_back( iBucket
                                      , FSnapshotFlags::Point::Bucket::PARAM
                                      , eSnapshotState::Initial );
}

// Backup bucket params in the constructor
FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorGroupPaint* iScene
                                                            , std::vector<FOdysseyVectorBucket*>& iAddedBucketArray
                                                            , std::vector<FOdysseyVectorBucket*>& iBucketArray
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mAddedBucketArray = iAddedBucketArray;

    mBucketSnapshotBuffer.reserve( iBucketArray.size() );

    for( int i = 0; i < iBucketArray.size(); i++ )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mBucketSnapshotBuffer.emplace_back( iBucketArray[i]
                                          , FSnapshotFlags::Point::Bucket::PARAM
                                          , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoBucketParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotBucket& bucketSnapshot : mBucketSnapshotBuffer )
    {
        bucketSnapshot.LoadState( eSnapshotState::Altered );
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoBucketParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotBucket& bucketSnapshot : mBucketSnapshotBuffer )
    {
        bucketSnapshot.RecordState( eSnapshotState::Altered );
    }


    for( FSnapshotBucket& bucketSnapshot : mBucketSnapshotBuffer )
    {
        bucketSnapshot.LoadState( eSnapshotState::Initial );
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketParam::ToString() const
{
    return FString("FOdysseyVectorUndoBucketParam");
}
