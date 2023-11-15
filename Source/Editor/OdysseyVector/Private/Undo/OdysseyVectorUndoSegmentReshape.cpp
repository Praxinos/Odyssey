#include "Undo/OdysseyVectorUndoSegmentReshape.h"

FOdysseyVectorUndoSegmentReshape::~FOdysseyVectorUndoSegmentReshape()
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

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene )
    : FOdysseyVectorUndo( iScene )
{
}

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                                                  , const std::vector<FOdysseyVectorVertex*>& iVertexArray )
    : FOdysseyVectorUndo( iScene )
{
    std::vector<FOdysseyVectorSegment*> segmentArray;

    FOdysseyVectorVertex::ArrayToSegmentArray( iVertexArray, segmentArray );

    mVertexSnapshotArray.reserve( iVertexArray.size() );

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        mVertexSnapshotArray.emplace_back( iVertexArray[i]
                                         , FSnapshotPoint::SNAPSHOT_ALL
                                         , FSnapshotVertex::SNAPSHOT_ALL );
    }

    RecordSegment( segmentArray );
}

FOdysseyVectorUndoSegmentReshape::FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                                                  , const std::vector<FOdysseyVectorSegment*>& iSegmentArray )
    : FOdysseyVectorUndo( iScene )
{
    RecordSegment( iSegmentArray );
}

void
FOdysseyVectorUndoSegmentReshape::RecordSegment( const std::vector<FOdysseyVectorSegment*>& iSegmentArray )
{
    mCubicSegmentSnapshotArray.reserve( iSegmentArray.size() );

    for( int i = 0; i < iSegmentArray.size(); i++ )
    {
        if( iSegmentArray[i]->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegmentArray[i]);

            mCubicSegmentSnapshotArray.emplace_back( cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_ALL );
        }
    }
}

void
FOdysseyVectorUndoSegmentReshape::RecordSegment( FOdysseyVectorSegment* iSegment )
{
    if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);

        mCubicSegmentSnapshotArray.emplace_back( cubicSegment, FSnapshotSegmentCubic::SNAPSHOT_ALL );
    }
}

bool
FOdysseyVectorUndoSegmentReshape::HasSegment( FOdysseyVectorSegment* iSegment )
{
    for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
    {
        if( mCubicSegmentSnapshotArray[i].GetCubicSegment() == iSegment )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorUndoSegmentReshape::Apply( UObject* iIgnored )
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
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoSegmentReshape::Revert( UObject* iIgnored )
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
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSegmentReshape::ToString() const
{
    return FString("FOdysseyVectorUndoSegmentReshape");
}
