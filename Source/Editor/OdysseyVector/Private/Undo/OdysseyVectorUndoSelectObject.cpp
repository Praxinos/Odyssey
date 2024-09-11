#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoSelectObject::~FOdysseyVectorUndoSelectObject()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorUndoSelectObject::FOdysseyVectorUndoSelectObject( FOdysseyVectorGroupPaint* iScene
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mSelectedObjectList = mScene->GetEngine()->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelectObject::Apply( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetEngine()->GetSelectedObjectList();

    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        mScene->GetEngine()->SelectObject( object );
    }

    // prepare former selection for Revert()
    mSelectedObjectList = selectedObjectList;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoSelectObject::Revert( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetEngine()->GetSelectedObjectList();
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        mScene->GetEngine()->SelectObject( object );
    }

    // prepare former selection for Apply()
    mSelectedObjectList = selectedObjectList;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectObject::ToString() const
{
    return FString("FOdysseyVectorUndoSelectObject");
}
