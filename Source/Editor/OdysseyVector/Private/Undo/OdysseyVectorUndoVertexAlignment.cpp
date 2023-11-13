#include "Undo/OdysseyVectorUndoVertexAlignment.h"

FOdysseyVectorUndoVertexAlignment::~FOdysseyVectorUndoVertexAlignment()
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

FOdysseyVectorUndoVertexAlignment::FOdysseyVectorUndoVertexAlignment( FOdysseyVectorScene* iScene
                                                                    , const std::vector<FOdysseyVectorVertex*>& iAlignedVertexArray )
    : FOdysseyVectorUndo( iScene )
{
    std::vector<FOdysseyVectorSegment*> segmentArray;

    //------ Backup vertex alignment flag part ---------//

    mVertexSnapshotArray.reserve( iAlignedVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iAlignedVertexArray )
    {
        mVertexSnapshotArray.emplace_back( vertex, 0, FSnapshotVertex::SNAPSHOT_ALIGNMENT );
    }

    //------ Backup segment handles part ---------//

    FOdysseyVectorVertex::ArrayToSegmentArray( iAlignedVertexArray, segmentArray );

    mCubicSegmentSnapshotArray.reserve( segmentArray.size() );

    for( FOdysseyVectorSegment* segment : segmentArray ) 
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            mCubicSegmentSnapshotArray.emplace_back( cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_HANDLES );
        }
    }
}

void
FOdysseyVectorUndoVertexAlignment::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotArray )
    {
        vertexSnapshot.Restore();
    }

    for( FSnapshotSegmentCubic& cubicSegmentsnapshot : mCubicSegmentSnapshotArray )
    {
        cubicSegmentsnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoVertexAlignment::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotArray )
    {
        vertexSnapshot.Restore();
    }

    for( FSnapshotSegmentCubic& cubicSegmentSnapshot : mCubicSegmentSnapshotArray )
    {
        cubicSegmentSnapshot.Restore();
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
FOdysseyVectorUndoVertexAlignment::ToString() const
{
    return FString("FOdysseyVectorUndoVertexAlignment");
}
