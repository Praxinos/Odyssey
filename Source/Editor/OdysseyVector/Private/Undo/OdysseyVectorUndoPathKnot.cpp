#include "Undo/OdysseyVectorUndoPathKnot.h"

FOdysseyVectorUndoPathKnot::~FOdysseyVectorUndoPathKnot()
{
    for( int i = 0; i < mMergedSegmentArray.size(); i++ )
    {
        delete mMergedSegmentArray[i];
    }

    for( int i = 0; i < mMergedVertexArray.size(); i++ )
    {
        delete mMergedVertexArray[i];
    }

    mMergedVertexArray.clear();
    mMergedSegmentArray.clear();
}

FOdysseyVectorUndoPathKnot::FOdysseyVectorUndoPathKnot( FOdysseyVectorScene* iScene
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
FOdysseyVectorUndoPathKnot::Apply( UObject* iIgnored )
{
    for( int i = 0; i < mMergedVertexArray.size(); i++ )
    {
        mMergedVertexArray[i]->GetPath()->AddVertex( mMergedVertexArray[i] );
    }

    for( int i = 0; i < mMergedSegmentArray.size(); i++ )
    {
        mMergedSegmentArray[i]->GetPath()->AddSegment( mMergedSegmentArray[i] );

        //mMergedSegmentArray[i]->Invalidate();
    }

    FOdysseyVectorUndoPathAlter::Apply( iIgnored );

    mScene->Update( 0 );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoPathKnot::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndoPathAlter::Revert( iIgnored );

    for( int i = 0; i < mMergedSegmentArray.size(); i++ )
    {
        mMergedSegmentArray[i]->GetPath()->RemoveSegment( mMergedSegmentArray[i] );
    }

    for( int i = 0; i < mMergedVertexArray.size(); i++ )
    {
        mMergedVertexArray[i]->GetPath()->RemoveVertex( mMergedVertexArray[i] );
    }

    // Update the bbox
    mScene->Update( 0 );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathKnot::ToString() const
{
    return FString("FOdysseyVectorUndoPathKnot");
}
