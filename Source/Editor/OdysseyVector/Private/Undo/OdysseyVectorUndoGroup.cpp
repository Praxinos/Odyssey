#include "Undo/OdysseyVectorUndoGroup.h"

FOdysseyVectorUndoGroup::~FOdysseyVectorUndoGroup()
{
    // if Grouping action confirmed
    if( mApplied )
    {
        // free memory for former groups
        for( int i = 0; i < mRemovedGroupArray.size(); i++ )
        {
            delete mRemovedGroupArray[i];
        }
    }
    else
    {
        // free memory for the created group only
        delete mAddedGroup;
    }

    mGroupRecordArray.clear();
    mRemovedGroupArray.clear();
}

FOdysseyVectorUndoGroup::FOdysseyVectorUndoGroup( FOdysseyVectorScene* iScene, bool iSwitchSpace )
    : FOdysseyVectorUndo()
    , mScene( iScene )
    , mSwitchSpace( iSwitchSpace )
{
}

void
FOdysseyVectorUndoGroup::RecordBefore( std::list<FOdysseyVectorObject*>& iGroupedObjectList )
{
    mGroupRecordArray.reserve( iGroupedObjectList.size() );

    for( std::list<FOdysseyVectorObject*>::iterator it = iGroupedObjectList.begin(); it != iGroupedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *object = (*it);

        mGroupRecordArray.push_back( FGroupRecord( object ) );
    }
}

void
FOdysseyVectorUndoGroup::RecordAfter( FOdysseyVectorGroup* iAddedGroup, std::vector<FOdysseyVectorGroup*>& iRemovedGroupArray )
{
    mAddedGroup = iAddedGroup;
    mRemovedGroupArray = iRemovedGroupArray;
}

void
FOdysseyVectorUndoGroup::RecordAfter( FOdysseyVectorGroup* iAddedGroup )
{
    mAddedGroup = iAddedGroup;
}

void
FOdysseyVectorUndoGroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->ClearSelection();

    // destroy the former hierarchy.
    for( int i = 0; i < mGroupRecordArray.size(); i++ )
    {
        mGroupRecordArray[i].parent->RemoveChild( mGroupRecordArray[i].object );
    }

    // remove all former groups
    for( int i = 0; i < mRemovedGroupArray.size(); i++ )
    {
        mRemovedGroupArray[i]->GetParent()->RemoveChild( mRemovedGroupArray[i] );
    }

    // Add the created group
    mAddedGroup->GetParent()->AppendChild( mAddedGroup );

    // Add all children to the newly created group
    for( int i = 0; i < mGroupRecordArray.size(); i++ )
    {
        mAddedGroup->AppendChild( mGroupRecordArray[i].object );

        mGroupRecordArray[i].object->UpdateMatrix();
        mGroupRecordArray[i].object->Invalidate();
    }

    // update invalidated objects
    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoGroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    // Remove all children from the created group
    for( int i = 0; i < mGroupRecordArray.size(); i++ )
    {
        mAddedGroup->RemoveChild( mGroupRecordArray[i].object );
    }

    // Remove the created group
    mAddedGroup->GetParent()->RemoveChild( mAddedGroup );

    // add all former groups back
    for( int i = 0; i < mRemovedGroupArray.size(); i++ )
    {
        // Note: the pointer to the parent is still valid although the object is technically orphan. We reuse it.
        mRemovedGroupArray[i]->GetParent()->AppendChild( mRemovedGroupArray[i] );
    }

    // reconstruct the former hierarchy.
    for( int i = 0; i < mGroupRecordArray.size(); i++ )
    {
        mGroupRecordArray[i].parent->AppendChild( mGroupRecordArray[i].object );

        mGroupRecordArray[i].object->UpdateMatrix();
        mGroupRecordArray[i].object->Invalidate();
    }

    // update invalidated objects
    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoGroup::ToString() const
{
    return FString("FOdysseyVectorUndoGroup");
}
