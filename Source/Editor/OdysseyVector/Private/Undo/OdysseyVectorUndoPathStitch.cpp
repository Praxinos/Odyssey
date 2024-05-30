#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoPathStitch::~FOdysseyVectorUndoPathStitch()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        for( int i = 0; i < mMergedSegmentArray.size(); i++ )
        {
            //if( 
            delete mMergedSegmentArray[i];
        }

        for( int i = 0; i < mMergedVertexArray.size(); i++ )
        {
            delete mMergedVertexArray[i];
        }
    }

    mMergedVertexArray.clear();
    mMergedSegmentArray.clear();
}

FOdysseyVectorUndoPathStitch::FOdysseyVectorUndoPathStitch( FOdysseyVectorGroupPaint* iScene
                                                          , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                                          , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                                          , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                                          , std::vector<FOdysseyVectorPath*>& iAddedPathArray
                                                          , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                          , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                                          , std::vector<FOdysseyVectorVertex*>& iMergedVertexArray
                                                          , std::vector<FOdysseyVectorSegment*>& iMergedSegmentArray )
    : FOdysseyVectorUndoPathAlter( iScene
                                 , iRemovedPathArray
                                 , iRemovedVertexArray
                                 , iRemovedSegmentArray
                                 , iAddedPathArray
                                 , iAddedVertexArray
                                 , iAddedSegmentArray )
{
    mMergedVertexArray = iMergedVertexArray;
    mMergedSegmentArray = iMergedSegmentArray;
}

void
FOdysseyVectorUndoPathStitch::Apply( UObject* iIgnored )
{
    for( int i = 0; i < mMergedVertexArray.size(); i++ )
    {
        mMergedVertexArray[i]->GetOwnerAsPath()->AddVertex( mMergedVertexArray[i] );
    }

    for( int i = 0; i < mMergedSegmentArray.size(); i++ )
    {
        mMergedSegmentArray[i]->GetOwnerAsPath()->AddSegment( mMergedSegmentArray[i] );

        //mMergedSegmentArray[i]->Invalidate();
    }

    FOdysseyVectorUndoPathAlter::Apply( iIgnored );

    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoPathStitch::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndoPathAlter::Revert( iIgnored );

    for( int i = 0; i < mMergedSegmentArray.size(); i++ )
    {
        mMergedSegmentArray[i]->GetOwnerAsPath()->RemoveSegment( mMergedSegmentArray[i] );
    }

    for( int i = 0; i < mMergedVertexArray.size(); i++ )
    {
        mMergedVertexArray[i]->GetOwnerAsPath()->RemoveVertex( mMergedVertexArray[i] );
    }

    // Update the bbox
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathStitch::ToString() const
{
    return FString("FOdysseyVectorUndoPathStitch");
}
