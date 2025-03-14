// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    mBucketSnapshotArray.push_back( FSnapshotBucket( iBucket, FSnapshotFlags::Point::Bucket::PARAM ));
}

// Backup bucket params in the constructor
FOdysseyVectorUndoBucketParam::FOdysseyVectorUndoBucketParam( FOdysseyVectorGroupPaint* iScene
                                                            , std::vector<FOdysseyVectorBucket*>& iAddedBucketArray
                                                            , std::vector<FOdysseyVectorBucket*>& iBucketArray
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketParam::ToString() const
{
    return FString("FOdysseyVectorUndoBucketParam");
}
