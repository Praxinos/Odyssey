// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoUngroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

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
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mGroup( iGroup )
    , mScene( iScene )
{
    mUngroupedObjectList = iGroup->GetChildrenList();
}

void
FOdysseyVectorUndoUngroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorObject* groupParent = mGroup->GetParent();

    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

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

    mScene->GetCell()->ClearObjectSelection();

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
