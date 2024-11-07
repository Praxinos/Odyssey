#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoPathAlter::~FOdysseyVectorUndoPathAlter()
{
    if( mApplied )
    {
        // Free paths
        for( int i = 0; i < mRemovedPathArray.size(); i++ )
        {
            delete mRemovedPathArray[i];
        }

        // Free vertices
        for( int i = 0; i < mRemovedVertexArray.size(); i++ )
        {
            delete mRemovedVertexArray[i];
        }

        // Free segments
        for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
        {
            delete mRemovedSegmentArray[i];
        }
    }
    else
    {
        // Free paths that were undone
        for( int i = 0; i < mAddedPathArray.size(); i++ )
        {
            delete mAddedPathArray[i];
        }

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

    mRemovedPathArray.clear();
    mRemovedVertexArray.clear();
    mRemovedSegmentArray.clear();
    mAddedPathArray.clear();
    mAddedVertexArray.clear();
    mAddedSegmentArray.clear();
}

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                                        , FOdysseyVectorVertex* iRemovedVertex
                                                        , FOdysseyVectorSegment* iRemovedSegment
                                                        , FOdysseyVectorVertex* iAddedVertex
                                                        , FOdysseyVectorSegment* iAddedSegment )
    : FOdysseyVectorUndo( iScene )
{
    if( iRemovedVertex ) 
        mRemovedVertexArray.push_back( iRemovedVertex );

    if( iRemovedSegment ) 
        mRemovedSegmentArray.push_back( iRemovedSegment );

    if( iAddedVertex )
        mAddedVertexArray.push_back( iAddedVertex );

    if( iAddedSegment )
        mAddedSegmentArray.push_back( iAddedSegment );
}

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                                        , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                                        , std::vector<FOdysseyVectorPath*>& iAddedPathArray
                                                        , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
    : FOdysseyVectorUndoPathAlter( iScene, iAddedPathArray, iAddedVertexArray, iAddedSegmentArray )
{
    mRemovedPathArray = iRemovedPathArray;
    mRemovedVertexArray = iRemovedVertexArray;
    mRemovedSegmentArray = iRemovedSegmentArray;
}

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorPath*>& iAddedPathArray
                                                        , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
    : FOdysseyVectorUndo( iScene )
{
    mAddedPathArray = iAddedPathArray;
    mAddedVertexArray = iAddedVertexArray;
    mAddedSegmentArray = iAddedSegmentArray;
}

void
FOdysseyVectorUndoPathAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
    {
        mRemovedSegmentArray[i]->GetOwnerAsPath()->RemoveSegment( mRemovedSegmentArray[i] );
    }

    for( int i = 0; i < mRemovedVertexArray.size(); i++ )
    {
        mRemovedVertexArray[i]->GetOwnerAsPath()->RemoveVertex( mRemovedVertexArray[i] );
    }

    for( int i = 0; i < mRemovedPathArray.size(); i++ )
    {
        mRemovedPathArray[i]->GetParent()->RemoveChild( mRemovedPathArray[i] );
    }


    for( int i = 0; i < mAddedPathArray.size(); i++ )
    {
        mAddedPathArray[i]->GetParent()->AppendChild( mAddedPathArray[i] );
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mAddedVertexArray[i]->GetOwnerAsPath()->AddVertex( mAddedVertexArray[i] );
    }

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mAddedSegmentArray[i]->GetOwnerAsPath()->AddSegment( mAddedSegmentArray[i] );
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
FOdysseyVectorUndoPathAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mRemovedPathArray.size(); i++ )
    {
        mRemovedPathArray[i]->GetParent()->AppendChild( mRemovedPathArray[i] );
    }

    for( int i = 0; i < mRemovedVertexArray.size(); i++ )
    {
        mRemovedVertexArray[i]->GetOwnerAsPath()->AddVertex( mRemovedVertexArray[i] );
    }

    for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
    {
        mRemovedSegmentArray[i]->GetOwnerAsPath()->AddSegment( mRemovedSegmentArray[i] );
    }


    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mAddedSegmentArray[i]->GetOwnerAsPath()->RemoveSegment( mAddedSegmentArray[i] );
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mAddedVertexArray[i]->GetOwnerAsPath()->RemoveVertex( mAddedVertexArray[i] );
    }

    for( int i = 0; i < mAddedPathArray.size(); i++ )
    {
        mAddedPathArray[i]->GetParent()->RemoveChild( mAddedPathArray[i] );
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
FOdysseyVectorUndoPathAlter::ToString() const
{
    return FString("FOdysseyVectorUndoPathAlter");
}
