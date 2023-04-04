#include "Undo/OdysseyVectorUndoPathAlter.h"

FOdysseyVectorUndoPathAlter::~FOdysseyVectorUndoPathAlter()
{
    if( mApplied )
    {
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

    mRemovedVertexArray.clear();
    mRemovedSegmentArray.clear();
    mAddedVertexArray.clear();
    mAddedSegmentArray.clear();
}

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorScene* iScene
                                                        , FOdysseyVectorVertex* iRemovedVertex
                                                        , FOdysseyVectorSegment* iRemovedSegment
                                                        , FOdysseyVectorVertex* iAddedVertex
                                                        , FOdysseyVectorSegment* iAddedSegment )
    : FOdysseyVectorUndo()
    , mScene( iScene )
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

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorScene* iScene
                                                        , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                                        , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
    : FOdysseyVectorUndoPathAlter( iScene, iAddedVertexArray, iAddedSegmentArray )
{
    mRemovedVertexArray = iRemovedVertexArray;
    mRemovedSegmentArray = iRemovedSegmentArray;
}

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorScene* iScene
                                                        , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
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
        mRemovedSegmentArray[i]->GetPath()->RemoveSegment( mRemovedSegmentArray[i] );
    }

    for( int i = 0; i < mRemovedVertexArray.size(); i++ )
    {
        mRemovedVertexArray[i]->GetPath()->RemoveVertex( mRemovedVertexArray[i] );
    }


    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mAddedVertexArray[i]->GetPath()->AddVertex( mAddedVertexArray[i] );
    }

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mAddedSegmentArray[i]->GetPath()->AddSegment( mAddedSegmentArray[i] );
    }

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoPathAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mRemovedVertexArray.size(); i++ )
    {
        mRemovedVertexArray[i]->GetPath()->AddVertex( mRemovedVertexArray[i] );
    }

    for( int i = 0; i < mRemovedSegmentArray.size(); i++ )
    {
        mRemovedSegmentArray[i]->GetPath()->AddSegment( mRemovedSegmentArray[i] );
    }


    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mAddedSegmentArray[i]->GetPath()->RemoveSegment( mAddedSegmentArray[i] );
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mAddedVertexArray[i]->GetPath()->RemoveVertex( mAddedVertexArray[i] );
    }

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathAlter::ToString() const
{
    return FString("FOdysseyVectorUndoPathAlter");
}
