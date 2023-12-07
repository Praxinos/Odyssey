#include "Undo/OdysseyVectorUndoPathEdit.h"

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

FOdysseyVectorUndoPathEdit::FOdysseyVectorUndoPathEdit( FOdysseyVectorGroupPaint* iScene
                                                      , const std::vector<FOdysseyVectorVertex*>& iEditedVertexArray
                                                      , const std::vector<FOdysseyVectorSegment*>& iEditedSegmentArray )
    : FOdysseyVectorUndo( iScene )
{
    mVertexSnapshotArray.reserve( iEditedVertexArray.size() );
    mCubicSegmentSnapshotArray.reserve( iEditedSegmentArray.size() );

    for( FOdysseyVectorVertex* vertex : iEditedVertexArray )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( vertex
                                                        , FSnapshotVertex::SNAPSHOT_POSITION
                                                        | FSnapshotVertex::SNAPSHOT_RADIUS
                                                        , 0 ));
    }

    for( FOdysseyVectorSegment* segment : iEditedSegmentArray )
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_HANDLES ));
        }
    }
}

void
FOdysseyVectorUndoPathEdit::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
    {
        mCubicSegmentSnapshotArray[i].Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoPathEdit::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
    {
        mCubicSegmentSnapshotArray[i].Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathEdit::ToString() const
{
    return FString("FOdysseyVectorUndoPathEdit");
}
