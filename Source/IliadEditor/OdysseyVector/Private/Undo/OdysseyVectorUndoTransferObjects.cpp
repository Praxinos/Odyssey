// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorRoot.h"
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
    , mScene( iScene )
{
    mTransferredObjectSnapshotArray.emplace_back( iTransferredObject, FSnapshotFlags::Object::HIERARCHY );
}

FOdysseyVectorUndoTransferObjects::FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                                                    , const std::list<FOdysseyVectorObject*>& iTransferredObjectList
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mScene( iScene )
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

    mScene->GetRoot()->ClearObjectSelection();

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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTransferObjects::Revert( UObject* iIgnored )
{
    bool allRestored = false;

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->GetRoot()->ClearObjectSelection();

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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTransferObjects::ToString() const
{
    return FString("FOdysseyVectorUndoTransferObjects");
}
