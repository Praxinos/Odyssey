#include "Undo/OdysseyVectorUndoBucketAdd.h"

#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoBucketAdd::~FOdysseyVectorUndoBucketAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        for( int i = 0; i < mBucketArray.size(); i++ )
        {
            delete mBucketArray[i];
        }
    }
}

FOdysseyVectorUndoBucketAdd::FOdysseyVectorUndoBucketAdd( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorBucket*>& iBucketArray
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mBucketArray = iBucketArray;
}

void
FOdysseyVectorUndoBucketAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mBucketArray.size(); i++ )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( mBucketArray[i]->GetOwner() );

        paintGroup->AddBucket( mBucketArray[i] );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoBucketAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for(int i = 0; i < mBucketArray.size(); i++)
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( mBucketArray[i]->GetOwner() );

        paintGroup->RemoveBucket( mBucketArray[i] );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketAdd::ToString() const
{
    return FString("FOdysseyVectorUndoBucketAdd");
}
