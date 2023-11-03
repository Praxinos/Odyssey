#include "Undo/OdysseyVectorUndoVertexAlign.h"

FOdysseyVectorUndoVertexAlign::~FOdysseyVectorUndoVertexAlign()
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
FOdysseyVectorUndoVertexAlign::IsSegmentRecorded( FOdysseyVectorSegment* iSegment )
{
    if( iSegment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);

        for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
        {
            if( mCubicSegmentSnapshotArray[i].GetCubicSegment() == cubicSegment )
            {
                return true;
            }
        }
    }

    return false;
}

FOdysseyVectorUndoVertexAlign::FOdysseyVectorUndoVertexAlign( FOdysseyVectorScene* iScene
                                                            , const std::vector<FOdysseyVectorVertex*>& iAlignedVertexArray )
    : FOdysseyVectorUndo( iScene )
{
    mAlignedVertexArray = iAlignedVertexArray;

    mCubicSegmentSnapshotArray.reserve( mAlignedVertexArray.size() * 2 );

    for( int i = 0; i < mAlignedVertexArray.size(); i++ )
    {
        for( FOdysseyVectorSegment* segment : mAlignedVertexArray[i]->GetSegmentList() ) 
        {
            if( IsSegmentRecorded( segment ) == false )
            {
                if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
                {
                    FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                    mCubicSegmentSnapshotArray.emplace_back( cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_HANDLES );
                }
            }
        }
    }
}

void
FOdysseyVectorUndoVertexAlign::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mAlignedVertexArray.size(); i++ )
    {
        mAlignedVertexArray[i]->SetHandleAligned( true );
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
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoVertexAlign::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mAlignedVertexArray.size(); i++ )
    {
        mAlignedVertexArray[i]->SetHandleAligned( false );
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
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexAlign::ToString() const
{
    return FString("FOdysseyVectorUndoVertexAlign");
}
