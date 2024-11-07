#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorUndoObjectTransform::~FOdysseyVectorUndoObjectTransform()
{
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
{
    mObjectSnapshotArray.push_back( FSnapshotObject( iObject, FSnapshotFlags::Object::TRANSFORMATIONS ) );
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
