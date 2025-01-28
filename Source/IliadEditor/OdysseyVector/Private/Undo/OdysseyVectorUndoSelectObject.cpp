// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoSelectObject::~FOdysseyVectorUndoSelectObject()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorUndoSelectObject::FOdysseyVectorUndoSelectObject( FOdysseyVectorGroupPaint* iScene
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mScene ( iScene )
{
    mSelectedObjectList = iScene->GetCell()->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelectObject::Apply( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetCell()->GetSelectedObjectList();

    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        mScene->GetCell()->SelectObject( object );
    }

    // prepare former selection for Revert()
    mSelectedObjectList = selectedObjectList;

    // force redraw
    mScene->GetLayer()->InvalidateCell( mScene->GetCell() );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoSelectObject::Revert( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetCell()->GetSelectedObjectList();
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        mScene->GetCell()->SelectObject( object );
    }

    // prepare former selection for Apply()
    mSelectedObjectList = selectedObjectList;

    // force redraw
    mScene->GetLayer()->InvalidateCell( mScene->GetCell() );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectObject::ToString() const
{
    return FString("FOdysseyVectorUndoSelectObject");
}
