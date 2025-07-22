// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

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
                                                              , std::vector<FOdysseyVectorPath*>& iPathArray
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mPathSnapshotBuffer.reserve( iPathArray.size() );

    for( int i = 0; i < iPathArray.size(); i++ )
    {
        mPathSnapshotBuffer.emplace_back( iPathArray[i]
                                        , FSnapshotFlags::Object::Path::VERTICES
                                        , eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mVertexSnapshotBuffer.reserve( iVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iVertexArray )
    {
        mVertexSnapshotBuffer.emplace_back( vertex
                                          , FSnapshotFlags::Point::Vertex::RADIUS
                                          , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoVertexRadius::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexSnapshotBuffer.size(); i++ )
    {
        mVertexSnapshotBuffer[i].LoadState( eSnapshotState::Altered );
    }

    for( int i = 0; i < mPathSnapshotBuffer.size(); i++ )
    {
        mPathSnapshotBuffer[i].LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoVertexRadius::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mVertexSnapshotBuffer.size(); i++ )
    {
        mVertexSnapshotBuffer[i].RecordState( eSnapshotState::Altered );
    }

    for( int i = 0; i < mPathSnapshotBuffer.size(); i++ )
    {
        mPathSnapshotBuffer[i].RecordState( eSnapshotState::Altered );
    }


    for( int i = 0; i < mVertexSnapshotBuffer.size(); i++ )
    {
        mVertexSnapshotBuffer[i].LoadState( eSnapshotState::Initial );
    }

    for( int i = 0; i < mPathSnapshotBuffer.size(); i++ )
    {
        mPathSnapshotBuffer[i].LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexRadius::ToString() const
{
    return FString("FOdysseyVectorUndoVertexRadius");
}
