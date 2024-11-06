#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

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
                                                          , std::vector<FOdysseyVectorSegment*>& iMergedSegmentArray
                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndoPathAlter( iScene
                                 , iRemovedPathArray
                                 , iRemovedVertexArray
                                 , iRemovedSegmentArray
                                 , iAddedPathArray
                                 , iAddedVertexArray
                                 , iAddedSegmentArray
                                 , iReturnFlags )
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

    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
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
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathStitch::ToString() const
{
    return FString("FOdysseyVectorUndoPathStitch");
}
