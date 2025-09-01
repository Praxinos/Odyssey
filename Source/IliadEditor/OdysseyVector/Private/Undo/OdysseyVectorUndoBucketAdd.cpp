// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoBucketAdd.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

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
                                                        , std::vector<FOdysseyVectorBucket*>& iBucketArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBucketAdd::ToString() const
{
    return FString("FOdysseyVectorUndoBucketAdd");
}
