#include "Undo/OdysseyVectorUndoRemoveObjects.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorSharedEnv.h"

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
                                                                , const std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mRemovedObjectArray = iRemovedObjectArray;
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

    mEngineList.front()->ClearObjectSelection();

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoRemoveObjects::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mEngineList.front()->ClearObjectSelection();

    for( FOdysseyVectorObject* object : mRemovedObjectArray )
    {
        object->GetOldParent()->AppendChild( object );

        //mScene->GetEngine()->SelectObject( object );
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoRemoveObjects::ToString() const
{
    return FString("FOdysseyVectorUndoRemoveObjects");
}
