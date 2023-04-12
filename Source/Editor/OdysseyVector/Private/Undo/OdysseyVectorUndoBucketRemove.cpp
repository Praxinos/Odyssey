#include "Undo/OdysseyVectorUndoBucketRemove.h"

FOdysseyVectorUndoBucketRemove::~FOdysseyVectorUndoBucketRemove()
{
    if( mApplied )
    {
        delete mBucket;
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoBucketRemove::FOdysseyVectorUndoBucketRemove( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorGroupPaint* iPaintGroup
                                                              , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo()
    , mScene( iScene )
    , mPaintGroup( iPaintGroup )
    , mBucket( iBucket )
{
}

void
FOdysseyVectorUndoBucketRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mPaintGroup->RemoveBucket( mBucket );
    mPaintGroup->Colorize();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoBucketRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mPaintGroup->AddBucket( mBucket );
    mPaintGroup->Colorize();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketRemove::ToString() const
{
    return FString("FOdysseyVectorUndoBucketRemove");
}
