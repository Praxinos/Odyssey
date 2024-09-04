#include "Undo/OdysseyVectorUndoSelectVertex.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoSelectVertex::~FOdysseyVectorUndoSelectVertex()
{
}

FOdysseyVectorUndoSelectVertex::FOdysseyVectorUndoSelectVertex( FOdysseyVectorGroupPaint* iScene
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

            mPathSnapshotArray.emplace_back( path, FSnapshotFlags::Object::Path::SELECTED_VERTICES );
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mPaintgroupSnapshotArray.emplace_back( paintGroup, FSnapshotFlags::Object::GroupPaint::SELECTED_BUCKETS );
        }
    }
}

void
FOdysseyVectorUndoSelectVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );
    
    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotArray )
    {
        paintgroupSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( 0 );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoSelectVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotArray )
    {
        paintgroupSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( 0 );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectVertex::ToString() const
{
    return FString("FOdysseyVectorUndoSelectVertex");
}
