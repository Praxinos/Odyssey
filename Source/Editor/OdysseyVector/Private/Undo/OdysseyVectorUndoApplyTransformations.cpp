#include "Undo/OdysseyVectorUndoApplyTransformations.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"

FOdysseyVectorUndoApplyTransformations::~FOdysseyVectorUndoApplyTransformations()
{
    for( int i = 0 ; i < mObjectSnapshotArray.size(); i++ )
    {
        delete mObjectSnapshotArray[i];
    }
}

FOdysseyVectorUndoApplyTransformations::FOdysseyVectorUndoApplyTransformations( FOdysseyVectorScene* iScene
                                                                              , std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mObjectSnapshotArray.push_back( new FSnapshotPath( path
                                                             , FSnapshotObject::SNAPSHOT_TRANSFORMATIONS
                                                             , FSnapshotPath::SNAPSHOT_VERTICES 
                                                             | FSnapshotPath::SNAPSHOT_SEGMENTS ) );
        }
        else
        if( object->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mObjectSnapshotArray.push_back( new FSnapshotGroupPaint( paintGroup
                                                                   , FSnapshotObject::SNAPSHOT_TRANSFORMATIONS
                                                                   | FSnapshotObject::SNAPSHOT_CHILDREN_TRANSFORMATIONS
                                                                   , FSnapshotGroupPaint::SNAPSHOT_BUCKETS ) );
        }
        else
        {
            mObjectSnapshotArray.push_back( new FSnapshotObject( object
                                                               , FSnapshotObject::SNAPSHOT_TRANSFORMATIONS ) );
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoApplyTransformations::ToString() const
{
    return FString("FOdysseyVectorUndoApplyTransformations");
}
