#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoObjectParam::~FOdysseyVectorUndoObjectParam()
{
    // free memory in all cases (applied or not)
    for( FSnapshotObject* objectSnapshot : mObjectSnapshotArray )
    {
        delete objectSnapshot;
    }

    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FSnapshotObject*
FOdysseyVectorUndoObjectParam::CreateObjectSnapshot( FOdysseyVectorObject* iObject )
{
    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        return new FSnapshotPath( path
                                , FSnapshotFlags::Object::PARAM
                                | FSnapshotFlags::Object::Path::PARAM );
    }
    else
    if( iObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);

        return new FSnapshotGroupPaint( paintGroup
                                      , FSnapshotFlags::Object::PARAM
                                      | FSnapshotFlags::Object::GroupPaint::PARAM );
    }

    return new FSnapshotObject( iObject, FSnapshotFlags::Object::PARAM );
}

// Backup bucket params in the constructor
FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , FOdysseyVectorObject* iObject
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectSnapshotArray.push_back( CreateObjectSnapshot( iObject ) );
}

// Backup bucket params in the constructor
FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , const std::vector<FOdysseyVectorObject*>& iObjectArray
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectSnapshotArray.reserve( iObjectArray.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectArray )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mObjectSnapshotArray.push_back( CreateObjectSnapshot( vectorObject ) );
    }
}

// Backup bucket params in the constructor
FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , const std::list<FOdysseyVectorObject*>& iObjectList
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        // Note: setting the owner does not make sense per se, as the bucket is only
        // temporary, but is mandatory in the ctor
        mObjectSnapshotArray.push_back( CreateObjectSnapshot( vectorObject ) );
    }
}

void
FOdysseyVectorUndoObjectParam::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->Restore();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoObjectParam::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->Restore();
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
FOdysseyVectorUndoObjectParam::ToString() const
{
    return FString("FOdysseyVectorUndoObjectParam");
}
