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
                                                        , FOdysseyVectorTag* iTag )
    : FOdysseyVectorUndo( iScene )
{
    mTagArray.push_back( iTag );
}

FOdysseyVectorUndoTagRemove::FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                                        , const std::vector<FOdysseyVectorTag*>& iTagArray )
    : FOdysseyVectorUndo( iScene )
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagRemove");
}
