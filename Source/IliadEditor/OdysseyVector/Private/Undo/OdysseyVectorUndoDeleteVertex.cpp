// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoDeleteVertex.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoDeleteVertex::~FOdysseyVectorUndoDeleteVertex()
{
}

FOdysseyVectorUndoDeleteVertex::FOdysseyVectorUndoDeleteVertex( FOdysseyVectorGroupPaint* iScene
                                                              , const std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mPathSnapshotBuffer.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mPathSnapshotBuffer.emplace_back( path
                                            , FSnapshotFlags::Object::Path::TOPOLOGY
                                            | FSnapshotFlags::Object::Path::SELECTED_VERTICES
                                            , eSnapshotState::Initial );
        }
    }
}

void
FOdysseyVectorUndoDeleteVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotBuffer )
    {
        pathSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoDeleteVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotBuffer )
    {
        pathSnapshot.RecordState( eSnapshotState::Altered );
    }

    for( FSnapshotPath& pathSnapshot : mPathSnapshotBuffer )
    {
        pathSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoDeleteVertex::ToString() const
{
    return FString("FOdysseyVectorUndoDeleteVertex");
}
