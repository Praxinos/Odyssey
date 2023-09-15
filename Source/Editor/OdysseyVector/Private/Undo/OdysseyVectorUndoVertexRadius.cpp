#include "Undo/OdysseyVectorUndoVertexRadius.h"

FOdysseyVectorUndoVertexRadius::~FOdysseyVectorUndoVertexRadius()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorScene* iScene
                                                              , std::vector<FOdysseyVectorPath*>& iPathArray )
    : FOdysseyVectorUndo( iScene )
{
    mPathSnapshotArray.reserve( iPathArray.size() );

    for( int i = 0; i < iPathArray.size(); i++ )
    {
        mPathSnapshotArray.emplace_back( iPathArray[i], 0, FSnapshotPath::SNAPSHOT_VERTICES );
    }
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorScene* iScene
                                                              , std::vector<FOdysseyVectorPoint*>& iPointArray )
    : FOdysseyVectorUndo( iScene )
{
    mVertexSnapshotArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        if( iPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>( iPointArray[i] );

            mVertexSnapshotArray.emplace_back( vertex
                                             , FSnapshotPoint::SNAPSHOT_RADIUS
                                             , FSnapshotVertex::SNAPSHOT_ALL );
        }
    }
}

void
FOdysseyVectorUndoVertexRadius::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mPathSnapshotArray.size(); i++ )
    {
        mPathSnapshotArray[i].Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoVertexRadius::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mPathSnapshotArray.size(); i++ )
    {
        mPathSnapshotArray[i].Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexRadius::ToString() const
{
    return FString("FOdysseyVectorUndoVertexRadius");
}
