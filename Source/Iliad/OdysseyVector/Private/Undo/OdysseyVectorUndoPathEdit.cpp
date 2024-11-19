// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoPathEdit.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

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
    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        if( mVertexSnapshotArray[i].GetVertex() == iVertex )
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
        for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
        {
            if( mCubicSegmentSnapshotArray[i].GetCubicSegment() == iSegment )
            {
                return true;
            }
        }
    }

    return false;
}

FOdysseyVectorUndoPathEdit::FOdysseyVectorUndoPathEdit( FOdysseyVectorGroupPaint* iScene
                                                      , const std::vector<FOdysseyVectorVertex*>& iEditedVertexArray
                                                      , const std::vector<FOdysseyVectorSegment*>& iEditedSegmentArray
                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mVertexSnapshotArray.reserve( iEditedVertexArray.size() );
    mCubicSegmentSnapshotArray.reserve( iEditedSegmentArray.size() );

    for( FOdysseyVectorVertex* vertex : iEditedVertexArray )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( vertex
                                                        , FSnapshotFlags::Point::POSITION
                                                        | FSnapshotFlags::Point::Vertex::RADIUS
                                                        | FSnapshotFlags::Point::Vertex::ALIGNMENT ) );
    }

    for( FOdysseyVectorSegment* segment : iEditedSegmentArray )
    {
        if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

            mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotFlags::Segment::Cubic::HANDLES ));
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
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
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
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathEdit::ToString() const
{
    return FString("FOdysseyVectorUndoPathEdit");
}
