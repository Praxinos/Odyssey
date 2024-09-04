#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "OdysseyVectorEngine.h"

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
                                                              , FOdysseyVectorBucket* iBucket
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mBucketArray.push_back( iBucket );
}

FOdysseyVectorUndoBucketRemove::FOdysseyVectorUndoBucketRemove( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorBucket*>& iBucketArray
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
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

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
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

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketRemove::ToString() const
{
    return FString("FOdysseyVectorUndoBucketRemove");
}
