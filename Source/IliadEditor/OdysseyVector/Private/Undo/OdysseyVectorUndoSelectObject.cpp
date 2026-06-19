// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoSelectObject::~FOdysseyVectorUndoSelectObject()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorUndoSelectObject::FOdysseyVectorUndoSelectObject( FOdysseyVectorLayer* iLayer
                                                              , FOdysseyVectorCell* iCell
                                                              , const std::list<FOdysseyVectorObject*>& iSelectedObjectList )
    : FOdysseyVectorUndo( iLayer )
    , mCellList ( { iCell } )
{
    mSelectedObjectList = iSelectedObjectList;
}

FOdysseyVectorUndoSelectObject::FOdysseyVectorUndoSelectObject( FOdysseyVectorLayer* iLayer
                                                              , FOdysseyVectorCell* iCell )
    : FOdysseyVectorUndo( iLayer )
    , mCellList ( { iCell } )
{
    GetSelectedObjectList( mSelectedObjectList );
}

FOdysseyVectorUndoSelectObject::FOdysseyVectorUndoSelectObject( FOdysseyVectorLayer* iLayer
                                                              , const std::list<FOdysseyVectorCell*>& iCellList )
    : FOdysseyVectorUndo( iLayer )
    , mCellList ( iCellList )
{
    GetSelectedObjectList( mSelectedObjectList );
}

void
FOdysseyVectorUndoSelectObject::GetSelectedObjectList( std::list<FOdysseyVectorObject*>& oObjectList )
{
    for( FOdysseyVectorCell* cell : mCellList )
    {
        for( FOdysseyVectorObject* selectedObject : cell->GetSelectedObjectList() )
        {
            oObjectList.push_back( selectedObject );
        }
    }
}

void
FOdysseyVectorUndoSelectObject::Apply( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList;

    GetSelectedObjectList( selectedObjectList );

    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorCell* cell : mCellList )
    {
        cell->ClearObjectSelection();
        // force redraw (for HUD)
        cell->GetLayer()->InvalidateCell( cell );
    }

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        object->GetCell()->SelectObject( object );
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
    std::list<FOdysseyVectorObject*> selectedObjectList;

    GetSelectedObjectList( selectedObjectList );

    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorCell* cell : mCellList )
    {
        cell->ClearObjectSelection();
        // force redraw (for HUD)
        cell->GetLayer()->InvalidateCell( cell );
    }

    for( FOdysseyVectorObject* object : mSelectedObjectList )
    {
        object->GetCell()->SelectObject( object );
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
