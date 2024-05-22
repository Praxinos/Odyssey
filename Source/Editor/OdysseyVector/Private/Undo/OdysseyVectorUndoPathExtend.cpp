#include "Undo/OdysseyVectorUndoPathExtend.h"

#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

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
                                                          , FOdysseyVectorPath* iPath )
    : FOdysseyVectorUndo( iScene )
    , mPath( iPath )
{
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
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
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathExtend::ToString() const
{
    return FString("FOdysseyVectorUndoPathExtend");
}
