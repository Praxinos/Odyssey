#include "Undo/OdysseyVectorUndoGroup.h"

FOdysseyVectorUndoGroup::~FOdysseyVectorUndoGroup()
{
    // if Grouping action confirmed
    if( mApplied )
    {
        // free memory for former buckets
        for( int i = 0; i < mRemovedBucketArray.size(); i++ )
        {
            delete mRemovedBucketArray[i];
        }
    }
    else
    {
        // free memory for the created group only
        delete mAddedGroup;
    }

    mAddedObjectOldParentArray.clear();
    mAddedObjectArray.clear();
    mRemovedBucketArray.clear();
}

FOdysseyVectorUndoGroup::FOdysseyVectorUndoGroup( FOdysseyVectorGroupPaint* iScene
                                                , FOdysseyVectorGroup* iAddedGroup
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectOldParentArray
                                                , std::vector<FOdysseyVectorBucket*>& iRemovedBucketArray  )
    : FOdysseyVectorUndo( iScene )
    , mAddedGroup( iAddedGroup )
{
    mRemovedBucketArray = iRemovedBucketArray;
    mAddedObjectOldParentArray = iAddedObjectOldParentArray;
    mAddedObjectArray = iAddedObjectArray;
}

FOdysseyVectorUndoGroup::FOdysseyVectorUndoGroup( FOdysseyVectorGroupPaint* iScene
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

    mScene->GetEngine()->ClearObjectSelection();

    // destroy the former hierarchy.
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedObjectOldParentArray[i]->RemoveChild( mAddedObjectArray[i] );
    }

    // remove all former buckets
    for( int i = 0; i < mRemovedBucketArray.size(); i++ )
    {
        FOdysseyVectorObject* ownerObject = mRemovedBucketArray[i]->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->RemoveBucket( mRemovedBucketArray[i] );
        }
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoGroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

    // Remove all children from the created group
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedGroup->RemoveChild( mAddedObjectArray[i] );
    }

    // Remove the created group
    mAddedGroup->GetParent()->RemoveChild( mAddedGroup );

    // add all former buckets back
    for( int i = 0; i < mRemovedBucketArray.size(); i++ )
    {
        FOdysseyVectorObject* ownerObject = mRemovedBucketArray[i]->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->AddBucket( mRemovedBucketArray[i] );
        }
    }

    // reconstruct the former hierarchy.
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedObjectOldParentArray[i]->AppendChild( mAddedObjectArray[i] );

        mAddedObjectArray[i]->UpdateMatrix();
        mAddedObjectArray[i]->Invalidate();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoGroup::ToString() const
{
    return FString("FOdysseyVectorUndoGroup");
}
