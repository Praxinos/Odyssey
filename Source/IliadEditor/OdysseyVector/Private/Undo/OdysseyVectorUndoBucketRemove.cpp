// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoBucketRemove::~FOdysseyVectorUndoBucketRemove()
{
    if( mApplied )
    {
        for( FOdysseyVectorBucket* bucket : mBucketArray )
        {
            delete bucket;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoBucketRemove::FOdysseyVectorUndoBucketRemove( FOdysseyVectorGroupPaint* iScene
                                                              , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mBucketArray.push_back( iBucket );
}

FOdysseyVectorUndoBucketRemove::FOdysseyVectorUndoBucketRemove( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorBucket*>& iBucketArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
    , mBucketArray( iBucketArray )
{
}

void
FOdysseyVectorUndoBucketRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorBucket* bucket : mBucketArray )
    {
        FOdysseyVectorObject* ownerObject = bucket->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->RemoveBucket( bucket );
        }
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoBucketRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorBucket* bucket : mBucketArray )
    {
        FOdysseyVectorObject* ownerObject = bucket->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->AddBucket( bucket );
        }
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketRemove::ToString() const
{
    return FString("FOdysseyVectorUndoBucketRemove");
}
