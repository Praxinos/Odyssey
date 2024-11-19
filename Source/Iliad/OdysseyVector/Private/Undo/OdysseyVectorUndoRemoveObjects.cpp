// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoRemoveObjects.h"

#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoRemoveObjects::~FOdysseyVectorUndoRemoveObjects()
{
    // Removal confirmed
    if( mApplied )
    {
        for( FOdysseyVectorObject* object : mRemovedObjectArray )
        {
            delete object;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoRemoveObjects::FOdysseyVectorUndoRemoveObjects( FOdysseyVectorGroupPaint* iScene
                                                                , const std::vector<FOdysseyVectorObject*>& iRemovedObjectArray )
    : FOdysseyVectorUndo( iScene )
{
    mRemovedObjectArray = iRemovedObjectArray;
}

void
FOdysseyVectorUndoRemoveObjects::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorObject* object : mRemovedObjectArray )
    {
        object->GetParent()->RemoveChild( object );
    }

    mScene->GetEngine()->ClearObjectSelection();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyVectorUndoRemoveObjects::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mRemovedObjectArray )
    {
        // Note: GetParent is still valid even though the object was removed from the children list.
        // This helps us to add the object to its parent anew without having to store the pointer to the parent object.
        object->GetParent()->AppendChild( object );

        //mScene->GetEngine()->SelectObject( object );
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
FOdysseyVectorUndoRemoveObjects::ToString() const
{
    return FString("FOdysseyVectorUndoRemoveObjects");
}
