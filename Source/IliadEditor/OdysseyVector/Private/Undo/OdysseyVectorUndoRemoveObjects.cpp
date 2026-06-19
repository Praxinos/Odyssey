// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoRemoveObjects.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h" // save inbetweeners
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
                                                                , const std::vector<FOdysseyVectorObject*>& iRemovedObjectArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
    , mCell ( iScene->GetCell() )
{
    std::list<FOdysseyVectorTagInbetweener*> inbetweenerTagList;

    mRemovedObjectArray = iRemovedObjectArray;

    GetInbetweenerTagList( iRemovedObjectArray, inbetweenerTagList );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : inbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                  , 0
                                                  , 0 )
                                                  .RecordState( eSnapshotState::Initial );
    }
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

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Altered );
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

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
    }

    for( FOdysseyVectorObject* object : mRemovedObjectArray )
    {
        object->GetOldParent()->AppendChild( object );

        //mScene->GetEngine()->SelectObject( object );
    }

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Initial );
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
