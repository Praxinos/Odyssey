// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoUngroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoUngroup::~FOdysseyVectorUndoUngroup()
{
    // if Grouping action confirmed
    if( mApplied )
    {
        //delete mGroup;
    }
    else
    {
        // nothing to do
    }

    mUngroupedObjectList.clear();
}

FOdysseyVectorUndoUngroup::FOdysseyVectorUndoUngroup( FOdysseyVectorGroupPaint* iScene
                                                    , FOdysseyVectorGroup* iGroup
                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mGroup( iGroup )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mUngroupedObjectList = iGroup->GetChildrenList();
}

void
FOdysseyVectorUndoUngroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorObject* groupParent = mGroup->GetParent();

    FOdysseyVectorUndo::Apply( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    for( FOdysseyVectorObject *child : mUngroupedObjectList )
    {
        // transfer the child to the group's parent object
        groupParent->TransferChild( child, groupParent->GetLastChild() );
    }

    //mGroup->GetParent()->RemoveChild( mGroup );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoUngroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    // Note: GetParent() stills holds a valid pointer to the former parent.
    //mGroup->GetParent()->AppendChild( mGroup );

    for( FOdysseyVectorObject *child : mUngroupedObjectList )
    {
        mGroup->TransferChild( child, mGroup->GetLastChild() );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoUngroup::ToString() const
{
    return FString("OdysseyVectorUndoUngroup");
}
