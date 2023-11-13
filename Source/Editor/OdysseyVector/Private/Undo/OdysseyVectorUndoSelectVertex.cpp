#include "Undo/OdysseyVectorUndoSelectVertex.h"

FOdysseyVectorUndoSelectVertex::~FOdysseyVectorUndoSelectVertex()
{
}

FOdysseyVectorUndoSelectVertex::FOdysseyVectorUndoSelectVertex( FOdysseyVectorScene* iScene
                                                              , const std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    mPathSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mPathSnapshotArray.emplace_back( path, 0, FSnapshotPath::SNAPSHOT_SELECTED_VERTICES );
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mPaintgroupSnapshotArray.emplace_back( paintGroup, 0, FSnapshotGroupPaint::SNAPSHOT_SELECTED_BUCKETS );
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
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
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
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectVertex::ToString() const
{
    return FString("FOdysseyVectorUndoSelectVertex");
}
