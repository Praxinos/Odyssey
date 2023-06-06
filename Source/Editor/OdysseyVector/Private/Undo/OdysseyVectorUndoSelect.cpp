#include "Undo/OdysseyVectorUndoSelect.h"

FOdysseyVectorUndoSelect::~FOdysseyVectorUndoSelect()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorUndoSelect::FOdysseyVectorUndoSelect( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo( iScene )
{
    mSelectedObjectList = mScene->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelect::Apply( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mScene->Select( object );
    }

    // prepare former selection for Revert()
    mSelectedObjectList = selectedObjectList;

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SCENE_REDRAW
                  | FOdysseyVectorScene::OBJECT_SELECTED );
}

void
FOdysseyVectorUndoSelect::Revert( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mScene->Select( object );
    }

    // prepare former selection for Apply()
    mSelectedObjectList = selectedObjectList;

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SCENE_REDRAW
                  | FOdysseyVectorScene::OBJECT_SELECTED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelect::ToString() const
{
    return FString("FOdysseyVectorUndoSelect");
}
