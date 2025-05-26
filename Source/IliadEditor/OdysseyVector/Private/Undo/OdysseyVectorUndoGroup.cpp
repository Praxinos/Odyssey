// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoGroup.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

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
                                                , std::vector<FOdysseyVectorBucket*>& iRemovedBucketArray
                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mAddedGroup( iAddedGroup )
    , mScene ( iScene )
{
    mRemovedBucketArray = iRemovedBucketArray;
    mAddedObjectArray = iAddedObjectArray;

    mAddedObjectOldParentArray.reserve( iAddedObjectArray.size() );

    for( FOdysseyVectorObject* vectorObject : iAddedObjectArray )
    {
        mAddedObjectOldParentArray.push_back( vectorObject->GetOldParent() );
    }
}

FOdysseyVectorUndoGroup::FOdysseyVectorUndoGroup( FOdysseyVectorGroupPaint* iScene
                                                , FOdysseyVectorGroup* iAddedGroup
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mAddedGroup( iAddedGroup )
    , mScene ( iScene )
{
    mAddedObjectArray = iAddedObjectArray;

    mAddedObjectOldParentArray.reserve( iAddedObjectArray.size() );

    for( FOdysseyVectorObject* vectorObject : iAddedObjectArray )
    {
        mAddedObjectOldParentArray.push_back( vectorObject->GetOldParent() );
    }
}

void
FOdysseyVectorUndoGroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

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
    mAddedGroup->GetOldParent()->AppendChild( mAddedGroup );

    // Add all children to the newly created group
    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedGroup->AppendChild( mAddedObjectArray[i] );

        mAddedObjectArray[i]->UpdateMatrix();
        //mAddedObjectArray[i]->Invalidate( INVALIDATE_SHAPE | INVALIDATE_COLOR | INVALIDATE_TOPOLOGY );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoGroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

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
        //mAddedObjectArray[i]->Invalidate();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoGroup::ToString() const
{
    return FString("FOdysseyVectorUndoGroup");
}
