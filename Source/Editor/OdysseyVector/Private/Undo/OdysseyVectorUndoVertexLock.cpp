#include "Undo/OdysseyVectorUndoVertexLock.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoVertexLock::~FOdysseyVectorUndoVertexLock()
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

FOdysseyVectorUndoVertexLock::FOdysseyVectorUndoVertexLock( FOdysseyVectorGroupPaint* iScene
                                                          , const std::vector<FOdysseyVectorVertex*>& iAlignedVertexArray
                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    //------ Backup vertex lock flag part ---------//

    mVertexSnapshotArray.reserve( iAlignedVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iAlignedVertexArray )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotFlags::Point::Vertex::LOCK ));
    }
}

void
FOdysseyVectorUndoVertexLock::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotArray )
    {
        vertexSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoVertexLock::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotVertex& vertexSnapshot : mVertexSnapshotArray )
    {
        vertexSnapshot.Restore();
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
FOdysseyVectorUndoVertexLock::ToString() const
{
    return FString("FOdysseyVectorUndoVertexLock");
}
