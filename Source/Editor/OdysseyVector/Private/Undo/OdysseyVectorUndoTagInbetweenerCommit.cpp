#include "Undo/OdysseyVectorUndoTagInbetweenerCommit.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerCommit::~FOdysseyVectorUndoTagInbetweenerCommit()
{
    if( mApplied )
    {
        for( FOdysseyVectorTag* tag : mRemovedTagList )
        {
            delete tag;
        }
    }
    else
    {
        for( FOdysseyVectorObject* object : mAddedObjectList )
        {
            delete object;
        }
    }
}

FOdysseyVectorUndoTagInbetweenerCommit::FOdysseyVectorUndoTagInbetweenerCommit( FOdysseyVectorGroupPaint* iScene
                                                                              , const std::list<FOdysseyVectorTag*>& iRemovedTagList 
                                                                              , const std::list<FOdysseyVectorObject*>& iAddedObjectList )
    : FOdysseyVectorUndo( iScene )
    , mRemovedTagList( iRemovedTagList )
    , mAddedObjectList( iAddedObjectList )
{
}

void
FOdysseyVectorUndoTagInbetweenerCommit::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorTag* tag : mRemovedTagList )
    {
        tag->GetOwner()->RemoveTag( tag );
    }

    for( FOdysseyVectorObject* object : mAddedObjectList )
    {
        object->GetOldParent()->AppendChild( object );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerCommit::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorTag* tag : mRemovedTagList )
    {
        tag->GetOwner()->AddTag( tag );
    }

    for( FOdysseyVectorObject* object : mAddedObjectList )
    {
        object->GetParent()->RemoveChild( object );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerCommit::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerCommit");
}
