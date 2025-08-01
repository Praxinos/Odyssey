// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTransferObjects::~FOdysseyVectorUndoTransferObjects()
{
    // action confirmed
    if( mApplied )
    {

    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTransferObjects::FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                                                    , FOdysseyVectorObject* iTransferredObject
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mScene( iScene )
{
    mTransferredObjectSnapshotBuffer.emplace_back( iTransferredObject
                                                 , FSnapshotFlags::Object::HIERARCHY
                                                 , eSnapshotState::Initial );
}

FOdysseyVectorUndoTransferObjects::FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                                                    , const std::list<FOdysseyVectorObject*>& iTransferredObjectList
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mScene( iScene )
{
    mTransferredObjectSnapshotBuffer.reserve( iTransferredObjectList.size() );

    for( FOdysseyVectorObject* transferredObject : iTransferredObjectList )
    {
        mTransferredObjectSnapshotBuffer.emplace_back( transferredObject
                                                     , FSnapshotFlags::Object::HIERARCHY
                                                     , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTransferObjects::Apply( UObject* iIgnored )
{
    bool allRestored = false;

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

    while( allRestored == false )
    {
        allRestored = true;

        for( FSnapshotObject& transferredObjectSnapshot : mTransferredObjectSnapshotBuffer )
        {
            if( transferredObjectSnapshot.LoadState( eSnapshotState::Altered ) == false )
            {
                // will tell the loop to continue until the hierarchy can be restored
                allRestored = false;
            }
        }
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTransferObjects::Revert( UObject* iIgnored )
{
    bool allRestored = false;

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetCell()->ClearObjectSelection();

    for( FSnapshotObject& transferredObjectSnapshot : mTransferredObjectSnapshotBuffer )
    {
        transferredObjectSnapshot.RecordState( eSnapshotState::Altered );
    }

    while( allRestored == false )
    {
        allRestored = true;

        for( FSnapshotObject& transferredObjectSnapshot : mTransferredObjectSnapshotBuffer )
        {
            if( transferredObjectSnapshot.LoadState( eSnapshotState::Initial ) == false )
            {
                // will tell the loop to continue until the hierarchy can be restored
                allRestored = false;
            }
        }
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTransferObjects::ToString() const
{
    return FString("FOdysseyVectorUndoTransferObjects");
}
