#include "Undo/OdysseyVectorUndoApplyTransformations.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoApplyTransformations::~FOdysseyVectorUndoApplyTransformations()
{
    for( int i = 0 ; i < mObjectSnapshotArray.size(); i++ )
    {
        delete mObjectSnapshotArray[i];
    }
}

FOdysseyVectorUndoApplyTransformations::FOdysseyVectorUndoApplyTransformations( FOdysseyVectorGroupPaint* iScene
                                                                              , std::list<FOdysseyVectorObject*>& iObjectList
                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mObjectSnapshotArray.push_back( new FSnapshotPath( path
                                                             , FSnapshotFlags::Object::TRANSFORMATIONS
                                                             | FSnapshotFlags::Object::Path::VERTICES
                                                             | FSnapshotFlags::Object::Path::SEGMENTS ) );
        }
        else
        if( object->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mObjectSnapshotArray.push_back( new FSnapshotGroupPaint( paintGroup
                                                                   , FSnapshotFlags::Object::TRANSFORMATIONS
                                                                   //| FSnapshotFlags::Object::CHILDREN_TRANSFORMATIONS
                                                                   | FSnapshotFlags::Object::GroupPaint::BUCKETS ) );
        }
        else
        {
            mObjectSnapshotArray.push_back( new FSnapshotObject( object
                                                               , FSnapshotFlags::Object::TRANSFORMATIONS ) );
        }
    }
}

void
FOdysseyVectorUndoApplyTransformations::Apply( UObject* iIgnored )
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
FOdysseyVectorUndoApplyTransformations::Revert( UObject* iIgnored )
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
FOdysseyVectorUndoApplyTransformations::ToString() const
{
    return FString("FOdysseyVectorUndoApplyTransformations");
}
