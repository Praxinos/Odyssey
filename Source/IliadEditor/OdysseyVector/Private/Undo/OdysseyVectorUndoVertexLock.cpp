// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoVertexLock.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoVertexLock::~FOdysseyVectorUndoVertexLock()
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

FOdysseyVectorUndoVertexLock::FOdysseyVectorUndoVertexLock( FOdysseyVectorGroupPaint* iScene
                                                          , const std::vector<FOdysseyVectorVertex*>& iAlignedVertexArray
                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    //------ Backup vertex lock flag part ---------//

    mVertexSnapshotBuffer.reserve( iAlignedVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iAlignedVertexArray )
    {
        mVertexSnapshotBuffer.emplace_back( vertex
                                          , FSnapshotFlags::Point::Vertex::LOCK
                                          , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoVertexLock::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    // restore altered state
    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotBuffer )
    {
        vertexSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoVertexLock::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    // record altered state
    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotBuffer )
    {
        vertexSnapshot.RecordState( eSnapshotState::Altered );
    }


    // restore initial state
    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotBuffer )
    {
        vertexSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexLock::ToString() const
{
    return FString("FOdysseyVectorUndoVertexLock");
}
