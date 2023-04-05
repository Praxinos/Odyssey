#include "Undo/OdysseyVectorUndoPointPosition.h"

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

    mPointPositionBeforeArray.clear();
    mPointPositionAfterArray.clear();
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
}

static void
RecordPosition( std::vector<FOdysseyVectorPoint*>& iPointArray, std::vector<FPointPosition>& mPointPositionArray )
{
    mPointPositionArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        mPointPositionArray.push_back( FPointPosition( iPointArray[i] ) );
    }
}

void
FOdysseyVectorUndoPointPosition::RecordBefore( std::vector<FOdysseyVectorPoint*>& iPointArray )
{
    RecordPosition( iPointArray, mPointPositionBeforeArray );
}

void
FOdysseyVectorUndoPointPosition::RecordAfter( std::vector<FOdysseyVectorPoint*>& iPointArray )
{
    RecordPosition( iPointArray, mPointPositionAfterArray );
}

static void
LoadArray( std::vector<FPointPosition>& mPointPositionArray )
{
    for( int i = 0; i < mPointPositionArray.size(); i++ )
    {
        mPointPositionArray[i].point->Set( mPointPositionArray[i].position.x, mPointPositionArray[i].position.y );

        if( mPointPositionArray[i].point->GetClass() == FOdysseyVectorVertexCubic::StaticClass() )
        {
            FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>( mPointPositionArray[i].point );

            cubicVertex->InvalidateSegments();
        }
        else
        if( mPointPositionArray[i].point->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( mPointPositionArray[i].point );

            segmentHandle->GetParent()->Invalidate();
        }
    }
}

void
FOdysseyVectorUndoPointPosition::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    LoadArray( mPointPositionAfterArray );

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoPointPosition::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    LoadArray( mPointPositionBeforeArray );

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPointPosition::ToString() const
{
    return FString("FOdysseyVectorUndoPointPosition");
}
