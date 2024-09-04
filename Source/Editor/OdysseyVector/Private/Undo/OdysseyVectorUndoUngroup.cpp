#include "Undo/OdysseyVectorUndoUngroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mGroup( iGroup )
{
    mUngroupedObjectList = iGroup->GetChildrenList();
}

void
FOdysseyVectorUndoUngroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorObject* groupParent = mGroup->GetParent();

    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

    for( FOdysseyVectorObject *child : mUngroupedObjectList )
    {
        // transfer the child to the group's parent object
        groupParent->TransferChild( child, groupParent->GetLastChild() );
    }

    //mGroup->GetParent()->RemoveChild( mGroup );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoUngroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

    // Note: GetParent() stills holds a valid pointer to the former parent.
    //mGroup->GetParent()->AppendChild( mGroup );

    for( FOdysseyVectorObject *child : mUngroupedObjectList )
    {
        mGroup->TransferChild( child, mGroup->GetLastChild() );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoUngroup::ToString() const
{
    return FString("OdysseyVectorUndoUngroup");
}
