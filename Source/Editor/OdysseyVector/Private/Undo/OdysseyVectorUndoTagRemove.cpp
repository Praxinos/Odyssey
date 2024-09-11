#include "Undo/OdysseyVectorUndoTagRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagRemove::~FOdysseyVectorUndoTagRemove()
{
    if( mApplied )
    {
        for( FOdysseyVectorTag* tag : mTagArray )
        {
            delete tag;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , FOdysseyVectorTag* iTag
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mTagArray.push_back( iTag );
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , const std::vector<FOdysseyVectorTag*>& iTagArray
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mTagArray( iTagArray )
{
}

void
FOdysseyVectorUndoTagRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorTag* tag : mTagArray )
    {
        tag->GetOwner()->RemoveTag( tag );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorTag* tag : mTagArray )
    {
        tag->GetOwner()->AddTag( tag );
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
FOdysseyVectorUndoTagRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagRemove");
}
