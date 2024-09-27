#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

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
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mTransferredObjectSnapshotArray.emplace_back( iTransferredObject, FSnapshotFlags::Object::HIERARCHY );
}

FOdysseyVectorUndoTransferObjects::FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                                                    , const std::list<FOdysseyVectorObject*>& iTransferredObjectList
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    for( FOdysseyVectorObject* transferredObject : iTransferredObjectList )
    {
        mTransferredObjectSnapshotArray.emplace_back( transferredObject, FSnapshotFlags::Object::HIERARCHY );
    }
}

void
FOdysseyVectorUndoTransferObjects::Apply( UObject* iIgnored )
{
    bool allRestored = false;

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    while( allRestored == false )
    {
        allRestored = true;

        for( FSnapshotObject& transferredObjectSnapshot : mTransferredObjectSnapshotArray )
        {
            if( transferredObjectSnapshot.Restore() == false )
            {
                // will tell the loop to continue until the hierarchy can be restored
                allRestored = false;
            }
        }
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTransferObjects::Revert( UObject* iIgnored )
{
    bool allRestored = false;

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    while( allRestored == false )
    {
        allRestored = true;

        for( FSnapshotObject& transferredObjectSnapshot : mTransferredObjectSnapshotArray )
        {
            if( transferredObjectSnapshot.Restore() == false )
            {
                // will tell the loop to continue until the hierarchy can be restored
                allRestored = false;
            }
        }
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // call callbacks if any (for refreshing GUI e.g)
    InvalidateEngineList( 0 );

    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTransferObjects::ToString() const
{
    return FString("FOdysseyVectorUndoTransferObjects");
}
