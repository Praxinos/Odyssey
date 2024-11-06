#include "Undo/OdysseyVectorUndoPathExtend.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoPathExtend::~FOdysseyVectorUndoPathExtend()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // Free vertices that were undone
        for( int i = 0; i < mAddedVertexArray.size(); i++ )
        {
            delete mAddedVertexArray[i];
        }

        // Free segments that were undone
        for( int i = 0; i < mAddedSegmentArray.size(); i++ )
        {
            delete mAddedSegmentArray[i];
        }
    }

    mAddedVertexArray.clear();
    mAddedSegmentArray.clear();
}

FOdysseyVectorUndoPathExtend::FOdysseyVectorUndoPathExtend( FOdysseyVectorGroupPaint* iScene
                                                          , FOdysseyVectorPath* iPath
                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mPath( iPath )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );
}

void
FOdysseyVectorUndoPathExtend::RecordSegment( FOdysseyVectorSegment* iNewSegment
                                           , FOdysseyVectorVertex* iNewVertex )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorUndoPathExtend::RecordSegment);
    mAddedSegmentArray.push_back( iNewSegment );

    if( iNewVertex )
    {
        mAddedVertexArray.push_back( iNewVertex );
    }
}

void
FOdysseyVectorUndoPathExtend::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mPath->AddVertex( mAddedVertexArray[i] );
    }

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mPath->AddSegment( mAddedSegmentArray[i] );
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoPathExtend::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mPath->RemoveSegment( mAddedSegmentArray[i] );
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mPath->RemoveVertex( mAddedVertexArray[i] );
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
FOdysseyVectorUndoPathExtend::ToString() const
{
    return FString("FOdysseyVectorUndoPathExtend");
}
