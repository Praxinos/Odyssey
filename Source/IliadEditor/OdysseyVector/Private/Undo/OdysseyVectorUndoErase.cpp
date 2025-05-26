// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoErase.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"

FOdysseyVectorUndoErase::~FOdysseyVectorUndoErase()
{
    if( mApplied )
    {
        // Free segments
        for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
        {
            delete mRemovedSegmentArray[i];
        }

        // Free vertices
        for( int i = 0; i < mRemovedVertexArray.size(); i++ )
        {
            delete mRemovedVertexArray[i];
        }

        // Free objects
        for( int i = 0; i < mRemovedObjectArray.size(); i++ )
        {
            delete mRemovedObjectArray[i];
        }
    }
    else
    {
        // Free segments that were undone
        for( int i = 0; i < mAddedSegmentArray.size(); i++ )
        {
            delete mAddedSegmentArray[i];
        }

        // Free vertices that were undone
        for( int i = 0; i < mAddedVertexArray.size(); i++ )
        {
            delete mAddedVertexArray[i];
        }

        // Free objects
        for( int i = 0; i < mAddedObjectArray.size(); i++ )
        {
            delete mAddedObjectArray[i];
        }
    }

    mAddedObjectArray.clear();
    mAddedVertexArray.clear();
    mAddedSegmentArray.clear();
    mRemovedObjectArray.clear();
    mRemovedVertexArray.clear();
    mRemovedSegmentArray.clear();
}

FOdysseyVectorUndoErase::FOdysseyVectorUndoErase( FOdysseyVectorGroupPaint* iScene
                                                , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                                , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                                , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                                , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                                , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    // to restore the selection (we dont record the initial state, it is already altered at that point anyways)
    , mCellSnapshot( iScene->GetCell() )
{
    mAddedObjectArray = iAddedObjectArray;
    mAddedVertexArray = iAddedVertexArray;
    mAddedSegmentArray = iAddedSegmentArray;
    mRemovedObjectArray = iRemovedObjectArray;
    mRemovedVertexArray = iRemovedVertexArray;
    mRemovedSegmentArray = iRemovedSegmentArray;
}

void
FOdysseyVectorUndoErase::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
    {
        //mRemovedSegmentArray[i]->Invalidate();
        mRemovedSegmentArray[i]->GetOwnerAsPath()->RemoveSegment( mRemovedSegmentArray[i] );
    }

    for( int i = 0; i < mRemovedVertexArray.size(); i++ )
    {
        //mRemovedVertexArray[i]->InvalidateSegments();
        mRemovedVertexArray[i]->GetOwnerAsPath()->RemoveVertex( mRemovedVertexArray[i] );
    }

    for( int i = 0; i < mRemovedObjectArray.size(); i++ )
    {
        //mRemovedObjectArray[i]->Invalidate();
        mRemovedObjectArray[i]->GetParent()->RemoveChild( mRemovedObjectArray[i] );
    }

    //----

    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        mAddedObjectArray[i]->GetOldParent()->AppendChild( mAddedObjectArray[i] );
        //mAddedObjectArray[i]->Invalidate();
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mAddedVertexArray[i]->GetOwnerAsPath()->AddVertex( mAddedVertexArray[i] );
        //mAddedVertexArray[i]->InvalidateSegments();
    }

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mAddedSegmentArray[i]->GetOwnerAsPath()->AddSegment( mAddedSegmentArray[i] );
        //mAddedSegmentArray[i]->Invalidate();
    }

    mCellSnapshot.LoadState( eSnapshotState::Altered );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoErase::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mCellSnapshot.RecordState( eSnapshotState::Altered );

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        //mAddedSegmentArray[i]->Invalidate();
        mAddedSegmentArray[i]->GetOwnerAsPath()->RemoveSegment( mAddedSegmentArray[i] );
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        //mAddedVertexArray[i]->InvalidateSegments();
        mAddedVertexArray[i]->GetOwnerAsPath()->RemoveVertex( mAddedVertexArray[i] );
    }

    for( int i = 0; i < mAddedObjectArray.size(); i++ )
    {
        //mAddedObjectArray[i]->Invalidate();
        mAddedObjectArray[i]->GetParent()->GetCell()->UnselectObject( mAddedObjectArray[i] );
        mAddedObjectArray[i]->GetParent()->RemoveChild( mAddedObjectArray[i] );
    }

    //----

    for( int i = 0; i < mRemovedObjectArray.size(); i++ )
    {
        mRemovedObjectArray[i]->GetOldParent()->AppendChild( mRemovedObjectArray[i] );
        //mRemovedObjectArray[i]->Invalidate();
    }

    for( int i = 0; i < mRemovedVertexArray.size(); i++ )
    {
        mRemovedVertexArray[i]->GetOwnerAsPath()->AddVertex( mRemovedVertexArray[i] );
        //mRemovedVertexArray[i]->InvalidateSegments();
    }

    for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
    {
        mRemovedSegmentArray[i]->GetOwnerAsPath()->AddSegment( mRemovedSegmentArray[i] );
        //mRemovedSegmentArray[i]->Invalidate();
    }

    //mCellSnapshot.LoadState( eSnapshotState::Initial );
    mCellSnapshot.GetCell()->ClearObjectSelection();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoErase::ToString() const
{
    return FString("FOdysseyVectorUndoErase");
}
