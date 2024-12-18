// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoSelectObject::~FOdysseyVectorUndoSelectObject()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorUndoSelectObject::FOdysseyVectorUndoSelectObject( FOdysseyVectorGroupPaint* iScene
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mSelectedObjectList = mEngineList.front()->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelectObject::Apply( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mEngineList.front()->GetSelectedObjectList();

    FOdysseyVectorUndo::Apply( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        mEngineList.front()->SelectObject( object );
    }

    // prepare former selection for Revert()
    mSelectedObjectList = selectedObjectList;

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoSelectObject::Revert( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mEngineList.front()->GetSelectedObjectList();
    FOdysseyVectorUndo::Revert( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        mEngineList.front()->SelectObject( object );
    }

    // prepare former selection for Apply()
    mSelectedObjectList = selectedObjectList;

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectObject::ToString() const
{
    return FString("FOdysseyVectorUndoSelectObject");
}
