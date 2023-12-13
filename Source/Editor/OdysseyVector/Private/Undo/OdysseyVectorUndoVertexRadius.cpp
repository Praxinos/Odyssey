#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

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

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorPath*>& iPathArray )
    : FOdysseyVectorUndo( iScene )
{
    mPathSnapshotArray.reserve( iPathArray.size() );

    for( int i = 0; i < iPathArray.size(); i++ )
    {
        mPathSnapshotArray.push_back( FSnapshotPath( iPathArray[i], FSnapshotFlags::Object::Path::VERTICES ));
    }
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorVertex*>& iVertexArray )
    : FOdysseyVectorUndo( iScene )
{
    mVertexSnapshotArray.reserve( iVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iVertexArray )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotFlags::Point::RADIUS ));
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
