#include "Undo/OdysseyVectorUndoKnot.h"

FOdysseyVectorUndoKnot::~FOdysseyVectorUndoKnot()
{
    if( mApplied )
    {
        delete mMergedPath;
    }
    else
    {
        for( int i = 0; i < mMergedSegmentArray.size(); i++ )
        {
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

FOdysseyVectorUndoKnot::FOdysseyVectorUndoKnot( FOdysseyVectorScene* iScene
                                              , FOdysseyVectorPath* iPath
                                              , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                              , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                              , FOdysseyVectorPath* iMergedPath
                                              , std::vector<FOdysseyVectorVertex*>& iMergedVertexArray
                                              , std::vector<FOdysseyVectorSegment*>& iMergedSegmentArray )
    : FOdysseyVectorUndoPathAlter( iScene, iRemovedVertexArray, iRemovedSegmentArray, iAddedVertexArray, iAddedSegmentArray )
    , mPath( iPath )
    , mMergedPath( iMergedPath )
{
    mMergedVertexArray = iMergedVertexArray;
    mMergedSegmentArray = iMergedSegmentArray;
}

void
FOdysseyVectorUndoKnot::Apply( UObject* iIgnored )
{
    if( mMergedPath )
    {
        mMergedPath->GetParent()->RemoveChild( mMergedPath );

        for( int i = 0; i < mMergedVertexArray.size(); i++ )
        {
            mPath->AddVertex( mMergedVertexArray[i] );
        }

        for( int i = 0; i < mMergedSegmentArray.size(); i++ )
        {
            mPath->AddSegment( mMergedSegmentArray[i] );

            mMergedSegmentArray[i]->Invalidate();
        }
    }

    FOdysseyVectorUndoPathAlter::Apply( iIgnored );

    mScene->Update( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SCENE_REDRAW
                  | FOdysseyVectorScene::OBJECT_SELECTED
                  | FOdysseyVectorScene::OBJECT_MODIFIED
                  | FOdysseyVectorScene::OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoKnot::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndoPathAlter::Revert( iIgnored );

    if( mMergedPath )
    {
        for( int i = 0; i < mMergedSegmentArray.size(); i++ )
        {
            mPath->RemoveSegment( mMergedSegmentArray[i] );
        }

        for( int i = 0; i < mMergedVertexArray.size(); i++ )
        {
            mPath->RemoveVertex( mMergedVertexArray[i] );
        }

        mMergedPath->GetParent()->AppendChild( mMergedPath );
    }

    // Update the bbox
    mScene->Update( 0 );
    mScene->Signal( FOdysseyVectorScene::SCENE_REDRAW
                  | FOdysseyVectorScene::OBJECT_SELECTED
                  | FOdysseyVectorScene::OBJECT_MODIFIED
                  | FOdysseyVectorScene::OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoKnot::ToString() const
{
    return FString("FOdysseyVectorUndoKnot");
}
