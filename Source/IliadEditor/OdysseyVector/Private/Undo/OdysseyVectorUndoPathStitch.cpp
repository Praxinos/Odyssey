// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathStitch::ToString() const
{
    return FString("FOdysseyVectorUndoPathStitch");
}
