#include "Undo/OdysseyVectorUndoPathDrawing.h"

FOdysseyVectorUndoPathDrawing::~FOdysseyVectorUndoPathDrawing()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // Free vertices that were undone
        for( int i = 0; i < mVertexArray.size(); i++ )
        {
            delete mVertexArray[i];
        }

        // Free segments that were undone
        for( int i = 0; i < mSegmentArray.size(); i++ )
        {
            delete mSegmentArray[i];
        }
    }

    mVertexArray.clear();
    mSegmentArray.clear();
}

FOdysseyVectorUndoPathDrawing::FOdysseyVectorUndoPathDrawing( FOdysseyVectorPath* iPath
                                                            , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                            , std::vector<FOdysseyVectorSegment*>& iSegmentArray )
    : FOdysseyVectorUndo()
    , mPath( iPath )
{
    mVertexArray = iVertexArray;
    mSegmentArray = iSegmentArray;
}

void
FOdysseyVectorUndoPathDrawing::Apply( UObject* iIgnored )
{
    FOdysseyVectorScene* scene = mPath->GetScene();
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexArray.size(); i++ )
    {
        mPath->AddVertex( mVertexArray[i] );
    }

    for( int i = 0; i < mSegmentArray.size(); i++ )
    {
        mPath->AddSegment( mSegmentArray[i] );
    }

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( scene );
}

void
FOdysseyVectorUndoPathDrawing::Revert( UObject* iIgnored )
{
    FOdysseyVectorScene* scene = mPath->GetScene();
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mSegmentArray.size(); i++ )
    {
        mPath->RemoveSegment( mSegmentArray[i] );
    }

    for( int i = 0; i < mVertexArray.size(); i++ )
    {
        mPath->RemoveVertex( mVertexArray[i] );
    }

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( scene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathDrawing::ToString() const
{
    return FString("FOdysseyVectorUndoPathDrawing");
}
