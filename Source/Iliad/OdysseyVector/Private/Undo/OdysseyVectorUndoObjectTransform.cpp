// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoObjectTransform::~FOdysseyVectorUndoObjectTransform()
{
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , FOdysseyVectorObject* iObject
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectSnapshotArray.push_back( FSnapshotObject( iObject, FSnapshotFlags::Object::TRANSFORMATIONS ) );
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , std::list<FOdysseyVectorObject*>& iObjectList
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        mObjectSnapshotArray.push_back( FSnapshotObject( vectorObject, FSnapshotFlags::Object::TRANSFORMATIONS ) );
    }
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotObject& objectSnapshot : mObjectSnapshotArray )
    {
        objectSnapshot.Restore();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotObject& objectSnapshot : mObjectSnapshotArray )
    {
        objectSnapshot.Restore();
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
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
