// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoPointPosition::~FOdysseyVectorUndoPointPosition()
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

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , std::vector<FOdysseyVectorPoint*>& iPointArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mPointSnapshotBuffer.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        mPointSnapshotBuffer.emplace_back( iPointArray[i]
                                         , FSnapshotFlags::ALL
                                         , eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                                , std::vector<FOdysseyVectorHandleSegment*>& iHandleArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mPointSnapshotBuffer.reserve( iVertexArray.size() + iHandleArray.size() );

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        mPointSnapshotBuffer.emplace_back( iVertexArray[i]
                                         , FSnapshotFlags::ALL
                                         , eSnapshotState::Initial );
    }

    for( int i = 0; i < iHandleArray.size(); i++ )
    {
        mPointSnapshotBuffer.emplace_back( iHandleArray[i]
                                         , FSnapshotFlags::ALL
                                         , eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , FOdysseyVectorPoint* iPoint )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mPointSnapshotBuffer.emplace_back( iPoint
                                     , FSnapshotFlags::ALL
                                     , eSnapshotState::Initial );
}

void
FOdysseyVectorUndoPointPosition::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mPointSnapshotBuffer.size(); i++ )
    {
        mPointSnapshotBuffer[i].LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoPointPosition::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mPointSnapshotBuffer.size(); i++ )
    {
        mPointSnapshotBuffer[i].RecordState( eSnapshotState::Altered );
    }

    for( int i = 0; i < mPointSnapshotBuffer.size(); i++ )
    {
        mPointSnapshotBuffer[i].LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPointPosition::ToString() const
{
    return FString("FOdysseyVectorUndoPointPosition");
}
