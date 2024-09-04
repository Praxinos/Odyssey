#include "Undo/OdysseyVectorUndoDeleteVertex.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorUndoDeleteVertex::~FOdysseyVectorUndoDeleteVertex()
{
}

FOdysseyVectorUndoDeleteVertex::FOdysseyVectorUndoDeleteVertex( FOdysseyVectorGroupPaint* iScene
                                                              , const std::list<FOdysseyVectorObject*>& iObjectList
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mPathSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mPathSnapshotArray.emplace_back( path, FSnapshotFlags::Object::Path::TOPOLOGY );
        }
    }
}

void
FOdysseyVectorUndoDeleteVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );
    
    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoDeleteVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoDeleteVertex::ToString() const
{
    return FString("FOdysseyVectorUndoDeleteVertex");
}
