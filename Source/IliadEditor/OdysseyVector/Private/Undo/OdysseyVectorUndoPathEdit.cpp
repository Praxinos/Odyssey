// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoPathEdit.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoPathEdit::~FOdysseyVectorUndoPathEdit()
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

bool
FOdysseyVectorUndoPathEdit::HasRecordedVertex( FOdysseyVectorVertex* iVertex )
{
    for( int i = 0; i < mVertexSnapshotBuffer.size(); i++ )
    {
        if( mVertexSnapshotBuffer[i].GetVertex() == iVertex )
        {
            return true;
        }
    }

    return false;
}

bool
FOdysseyVectorUndoPathEdit::HasRecordedSegment( FOdysseyVectorSegment* iSegment )
{
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        for( int i = 0; i < mCubicSegmentSnapshotBuffer.size(); i++ )
        {
            if( mCubicSegmentSnapshotBuffer[i].GetCubicSegment() == iSegment )
            {
                return true;
            }
        }
    }

    return false;
}

FOdysseyVectorUndoPathEdit::FOdysseyVectorUndoPathEdit( FOdysseyVectorGroupPaint* iScene
                                                      , const std::vector<FOdysseyVectorVertex*>& iEditedVertexArray
                                                      , const std::vector<FOdysseyVectorSegment*>& iEditedSegmentArray )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mVertexSnapshotBuffer.reserve( iEditedVertexArray.size() );
    mCubicSegmentSnapshotBuffer.reserve( iEditedSegmentArray.size() );

    for( FOdysseyVectorVertex* vertex : iEditedVertexArray )
    {
        mVertexSnapshotBuffer.emplace_back( vertex
                                          , FSnapshotFlags::Point::POSITION
                                          | FSnapshotFlags::Point::Vertex::RADIUS
                                          | FSnapshotFlags::Point::Vertex::ALIGNMENT
                                          , eSnapshotState::Initial );
    }

    for( FOdysseyVectorSegment* segment : iEditedSegmentArray )
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            mCubicSegmentSnapshotBuffer.emplace_back( cubicSegment
                                                    , FSnapshotFlags::Segment::Cubic::HANDLES
                                                    , eSnapshotState::Initial );
        }
    }
}

void
FOdysseyVectorUndoPathEdit::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexSnapshotBuffer.size(); i++ )
    {
        mVertexSnapshotBuffer[i].LoadState( eSnapshotState::Altered );
    }

    for( int i = 0; i < mCubicSegmentSnapshotBuffer.size(); i++ )
    {
        mCubicSegmentSnapshotBuffer[i].LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoPathEdit::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    // remember altered state
    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotBuffer )
    {
        vertexSnapshot.RecordState( eSnapshotState::Altered );
    }

    for( FSnapshotSegmentCubic& cubicSegmentSnapshot : mCubicSegmentSnapshotBuffer )
    {
        cubicSegmentSnapshot.RecordState( eSnapshotState::Altered );
    }


    // restore initial state
    for( int i = 0; i < mVertexSnapshotBuffer.size(); i++ )
    {
        mVertexSnapshotBuffer[i].LoadState( eSnapshotState::Initial );
    }

    for( int i = 0; i < mCubicSegmentSnapshotBuffer.size(); i++ )
    {
        mCubicSegmentSnapshotBuffer[i].LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathEdit::ToString() const
{
    return FString("FOdysseyVectorUndoPathEdit");
}
