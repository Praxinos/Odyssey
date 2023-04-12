#include "Undo/OdysseyVectorUndoBucketAdd.h"

FOdysseyVectorUndoBucketAdd::~FOdysseyVectorUndoBucketAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        delete mBucket;
    }
}

FOdysseyVectorUndoBucketAdd::FOdysseyVectorUndoBucketAdd( FOdysseyVectorScene* iScene
                                                        , FOdysseyVectorGroupPaint* iPaintGroup
                                                        , FOdysseyVectorBucket* iBucket )
    : FOdysseyVectorUndo()
    , mScene( iScene )
    , mPaintGroup( iPaintGroup )
    , mBucket( iBucket )
{
}

void
FOdysseyVectorUndoBucketAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mPaintGroup->AddBucket( mBucket );
    mPaintGroup->Colorize();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoBucketAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mPaintGroup->RemoveBucket( mBucket );
    mPaintGroup->Colorize();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketAdd::ToString() const
{
    return FString("FOdysseyVectorUndoBucketAdd");
}
