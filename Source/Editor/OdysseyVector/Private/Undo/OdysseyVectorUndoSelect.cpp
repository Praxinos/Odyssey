#include "Undo/OdysseyVectorUndoSelect.h"

FOdysseyVectorUndoSelect::~FOdysseyVectorUndoSelect()
{
    mSelectedObjectBeforeList.clear();
    mSelectedObjectAfterList.clear();
}

FOdysseyVectorUndoSelect::FOdysseyVectorUndoSelect( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
}

void
FOdysseyVectorUndoSelect::RecordBefore()
{
    mSelectedObjectBeforeList = mScene->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelect::RecordAfter()
{
    mSelectedObjectAfterList = mScene->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelect::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectAfterList.begin(); it != mSelectedObjectAfterList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mScene->Select( object );
    }

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoSelect::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectBeforeList.begin(); it != mSelectedObjectBeforeList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mScene->Select( object );
    }

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelect::ToString() const
{
    return FString("FOdysseyVectorUndoSelect");
}
