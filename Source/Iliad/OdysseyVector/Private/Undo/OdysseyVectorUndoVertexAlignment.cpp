// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoVertexAlignment.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

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

FOdysseyVectorUndoVertexAlignment::FOdysseyVectorUndoVertexAlignment( FOdysseyVectorGroupPaint* iScene
                                                                    , const std::vector<FOdysseyVectorVertex*>& iAlignedVertexArray )
    : FOdysseyVectorUndo( iScene )
{
    std::vector<FOdysseyVectorSegment*> segmentArray;

    //------ Backup vertex alignment flag part ---------//

    mVertexSnapshotArray.reserve( iAlignedVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iAlignedVertexArray )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotFlags::Point::Vertex::ALIGNMENT ));
    }

    //------ Backup segment handles part ---------//

    FOdysseyVectorVertex::ArrayToSegmentArray( iAlignedVertexArray, segmentArray );

    mCubicSegmentSnapshotArray.reserve( segmentArray.size() );

    for( FOdysseyVectorSegment* segment : segmentArray )
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotFlags::Segment::Cubic::HANDLES ));
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
