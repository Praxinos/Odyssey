// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

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
                                                        , FOdysseyVectorSegment* iAddedSegment
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
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
                                                        , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndoPathAlter( iScene, iAddedPathArray, iAddedVertexArray, iAddedSegmentArray, iReturnFlags )
{
    mRemovedPathArray = iRemovedPathArray;
    mRemovedVertexArray = iRemovedVertexArray;
    mRemovedSegmentArray = iRemovedSegmentArray;
}

FOdysseyVectorUndoPathAlter::FOdysseyVectorUndoPathAlter( FOdysseyVectorGroupPaint* iScene
                                                        , std::vector<FOdysseyVectorPath*>& iAddedPathArray
                                                        , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                                        , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
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
        mAddedPathArray[i]->GetOldParent()->AppendChild( mAddedPathArray[i] );
    }

    for( int i = 0; i < mAddedVertexArray.size(); i++ )
    {
        mAddedVertexArray[i]->GetOwnerAsPath()->AddVertex( mAddedVertexArray[i] );
    }

    for( int i = 0; i < mAddedSegmentArray.size(); i++ )
    {
        mAddedSegmentArray[i]->GetOwnerAsPath()->AddSegment( mAddedSegmentArray[i] );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoPathAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mRemovedPathArray.size(); i++ )
    {
        mRemovedPathArray[i]->GetOldParent()->AppendChild( mRemovedPathArray[i] );
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPathAlter::ToString() const
{
    return FString("FOdysseyVectorUndoPathAlter");
}
