#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoPointPosition::~FOdysseyVectorUndoPointPosition()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , std::vector<FOdysseyVectorPoint*>& iPointArray
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mPointSnapshotArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        mPointSnapshotArray.push_back( FSnapshotPoint( iPointArray[i], FSnapshotFlags::ALL ) );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                                , std::vector<FOdysseyVectorHandleSegment*>& iHandleArray
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mPointSnapshotArray.reserve( iVertexArray.size() + iHandleArray.size() );

    for( int i = 0; i < iVertexArray.size(); i++ )
    {
        mPointSnapshotArray.push_back( FSnapshotPoint( iVertexArray[i], FSnapshotFlags::ALL ) );
    }

    for( int i = 0; i < iHandleArray.size(); i++ )
    {
        mPointSnapshotArray.push_back( FSnapshotPoint( iHandleArray[i], FSnapshotFlags::ALL ) );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                                                , FOdysseyVectorPoint* iPoint
                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mPointSnapshotArray.push_back( FSnapshotPoint( iPoint, FSnapshotFlags::ALL ) );
}

void
FOdysseyVectorUndoPointPosition::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mPointSnapshotArray.size(); i++ )
    {
        mPointSnapshotArray[i].Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoPointPosition::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mPointSnapshotArray.size(); i++ )
    {
        mPointSnapshotArray[i].Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPointPosition::ToString() const
{
    return FString("FOdysseyVectorUndoPointPosition");
}
