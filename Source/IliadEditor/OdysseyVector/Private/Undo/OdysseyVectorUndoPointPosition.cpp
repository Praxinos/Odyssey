// IDDN.FR.001.060015.013.S.X.2019.000.00000
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
                                                                , std::vector<FOdysseyVectorPoint*>& iPointArray
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mPointSnapshotArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        mPointSnapshotArray.push_back( FSnapshotPoint( iPointArray[i]
                                                     , FSnapshotFlags::ALL
                                                     , eSnapshotState::Initial ) );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                                , std::vector<FOdysseyVectorHandleSegment*>& iHandleArray
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mPointSnapshotArray.reserve( iVertexArray.size() + iHandleArray.size() );

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        mPointSnapshotArray.push_back( FSnapshotPoint( iVertexArray[i]
                                                     , FSnapshotFlags::ALL
                                                     , eSnapshotState::Initial ) );
    }

    for( int i = 0; i < iHandleArray.size(); i++ )
    {
        mPointSnapshotArray.push_back( FSnapshotPoint( iHandleArray[i]
                                                     , FSnapshotFlags::ALL
                                                     , eSnapshotState::Initial ) );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , FOdysseyVectorPoint* iPoint
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mPointSnapshotArray.push_back( FSnapshotPoint( iPoint
                                                 , FSnapshotFlags::ALL
                                                 , eSnapshotState::Initial ) );
}

void
FOdysseyVectorUndoPointPosition::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mPointSnapshotArray.size(); i++ )
    {
        mPointSnapshotArray[i].LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoPointPosition::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mPointSnapshotArray.size(); i++ )
    {
        mPointSnapshotArray[i].RecordState( eSnapshotState::Altered );
    }

    for( int i = 0; i < mPointSnapshotArray.size(); i++ )
    {
        mPointSnapshotArray[i].LoadState( eSnapshotState::Initial );
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
