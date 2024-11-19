// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

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
                                                            , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
{
    mObjectSnapshotArray.push_back( CreateObjectSnapshot( iObject ) );
}

// Backup bucket params in the constructor
FOdysseyVectorUndoObjectParam::FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                                            , const std::vector<FOdysseyVectorObject*>& iObjectArray )
    : FOdysseyVectorUndo( iScene )
{
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
                                                            , const std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectParam::ToString() const
{
    return FString("FOdysseyVectorUndoObjectParam");
}
