#include "Undo/OdysseyVectorUndoSceneRemoveSelection.h"

FOdysseyVectorUndoSceneRemoveSelection::~FOdysseyVectorUndoSceneRemoveSelection()
{
    // Removal confirmed
    if( mApplied )
    {
        for( FOdysseyVectorObject* object : mRemovedObjectList )
        {
            delete object;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoSceneRemoveSelection::FOdysseyVectorUndoSceneRemoveSelection( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo( iScene )
{
    mRemovedObjectList = mScene->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSceneRemoveSelection::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorObject* object : mRemovedObjectList )
    {
        object->GetParent()->RemoveChild( object );
    }

    mScene->ClearSelection();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyVectorUndoSceneRemoveSelection::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    for( FOdysseyVectorObject* object : mRemovedObjectList )
    {
        // Note: GetParent is still valid even though the object was removed from the children list.
        // This helps us to add the object to its parent anew without having to store the pointer to the parent object.
        object->GetParent()->AppendChild( object );

        mScene->Select( object );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSceneRemoveSelection::ToString() const
{
    return FString("FOdysseyVectorUndoSceneRemoveSelection");
}
