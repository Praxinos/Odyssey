#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mTransferredObjectSnapshotArray.emplace_back( iTransferredObject, FSnapshotFlags::Object::HIERARCHY );
}

FOdysseyVectorUndoTransferObjects::FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                                                    , const std::list<FOdysseyVectorObject*>& iTransferredObjectList
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
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

    mScene->GetEngine()->ClearObjectSelection();

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
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoTransferObjects::Revert( UObject* iIgnored )
{
    bool allRestored = false;

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetEngine()->ClearObjectSelection();

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
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTransferObjects::ToString() const
{
    return FString("FOdysseyVectorUndoTransferObjects");
}
