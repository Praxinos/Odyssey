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
    : FOdysseyVectorUndo( iScene )
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

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

void
FOdysseyVectorUndoBucketAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mPaintGroup->RemoveBucket( mBucket );
    mPaintGroup->Colorize();

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketAdd::ToString() const
{
    return FString("FOdysseyVectorUndoBucketAdd");
}
