#include "Undo/OdysseyVectorUndoGroup.h"

FOdysseyVectorUndoGroup::~FOdysseyVectorUndoGroup()
{
    // if Grouping action confirmed
    if( mApplied )
    {
        // free memory for former groups
        for( int i = 0; i < mRemovedObjectArray.size(); i++ )
        {
            delete mRemovedObjectArray[i];
        }
    }
    else
    {
        // free memory for the created group only
        delete mAddedGroup;
    }

    mRemovedObjectArray.clear();
    mAddedObjectOldParentArray.clear();
    mAddedObjectArray.clear();
}

FOdysseyVectorUndoGroup::FOdysseyVectorUndoGroup( FOdysseyVectorScene* iScene
                                                , FOdysseyVectorGroup* iAddedGroup
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectOldParentArray
                                                , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray  )
    : FOdysseyVectorUndo( iScene )
    , mAddedGroup( iAddedGroup )
{
    mRemovedObjectArray = iRemovedObjectArray;
    mAddedObjectOldParentArray = iAddedObjectOldParentArray;
    mAddedObjectArray = iAddedObjectArray;
}

FOdysseyVectorUndoGroup::FOdysseyVectorUndoGroup( FOdysseyVectorScene* iScene
                                                , FOdysseyVectorGroup* iAddedGroup
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectOldParentArray  )
    : FOdysseyVectorUndo( iScene )
    , mAddedGroup( iAddedGroup )
{
    mAddedObjectOldParentArray = iAddedObjectOldParentArray;
    mAddedObjectArray = iAddedObjectArray;
}

void
FOdysseyVectorUndoGroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->ClearSelection();

    // destroy the former hierarchy.
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedObjectOldParentArray[i]->RemoveChild( mAddedObjectArray[i] );
    }

    // remove all former groups
    for( int i = 0; i < mRemovedObjectArray.size(); i++ )
    {
        mRemovedObjectArray[i]->GetParent()->RemoveChild( mRemovedObjectArray[i] );
    }

    // Add the created group
    mAddedGroup->GetParent()->AppendChild( mAddedGroup );

    // Add all children to the newly created group
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedGroup->AppendChild( mAddedObjectArray[i] );

        mAddedObjectArray[i]->UpdateMatrix();
        mAddedObjectArray[i]->Invalidate();
    }

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNALL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNALL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNALL_OBJECT_MODIFIED
                  | FOdysseyVectorScene::SIGNALL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoGroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    // Remove all children from the created group
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedGroup->RemoveChild( mAddedObjectArray[i] );
    }

    // Remove the created group
    mAddedGroup->GetParent()->RemoveChild( mAddedGroup );

    // add all former groups back
    for( int i = 0; i < mRemovedObjectArray.size(); i++ )
    {
        // Note: the pointer to the parent is still valid although the object is technically orphan. We reuse it.
        mRemovedObjectArray[i]->GetParent()->AppendChild( mRemovedObjectArray[i] );
    }

    // reconstruct the former hierarchy.
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedObjectOldParentArray[i]->AppendChild( mAddedObjectArray[i] );

        mAddedObjectArray[i]->UpdateMatrix();
        mAddedObjectArray[i]->Invalidate();
    }

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNALL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNALL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNALL_OBJECT_MODIFIED
                  | FOdysseyVectorScene::SIGNALL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoGroup::ToString() const
{
    return FString("FOdysseyVectorUndoGroup");
}
