// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoVertexAlignment.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

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
                                                                    , const std::vector<FOdysseyVectorVertex*>& iAlignedVertexArray
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexAlignment::ToString() const
{
    return FString("FOdysseyVectorUndoVertexAlignment");
}
