// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoRemoveObjects.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

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
                                                                , const std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mCell ( iScene->GetCell() )
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

    mCell->ClearObjectSelection();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoRemoveObjects::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mCell->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mRemovedObjectArray )
    {
        object->GetOldParent()->AppendChild( object );

        //mScene->GetEngine()->SelectObject( object );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoRemoveObjects::ToString() const
{
    return FString("FOdysseyVectorUndoRemoveObjects");
}
