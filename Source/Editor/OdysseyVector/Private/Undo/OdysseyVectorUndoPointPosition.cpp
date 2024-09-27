#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"

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
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

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
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

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
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

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
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
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
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPointPosition::ToString() const
{
    return FString("FOdysseyVectorUndoPointPosition");
}
