// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoSelectVertex.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoSelectVertex::~FOdysseyVectorUndoSelectVertex()
{
}

FOdysseyVectorUndoSelectVertex::FOdysseyVectorUndoSelectVertex( FOdysseyVectorGroupPaint* iScene
                                                              , const std::list<FOdysseyVectorObject*>& iObjectList
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    uint32 pathCount = 0;
    uint32 groupCount = 0;

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            pathCount++;
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            groupCount++;
        }
    }

    mPathSnapshotBuffer.reserve( pathCount );
    mPaintgroupSnapshotBuffer.reserve( groupCount );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mPathSnapshotBuffer.emplace_back( path
                                            , FSnapshotFlags::Object::Path::SELECTED_VERTICES
                                            , eSnapshotState::Initial );
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mPaintgroupSnapshotBuffer.emplace_back( paintGroup
                                                  , FSnapshotFlags::Object::Group::Paint::SELECTED_BUCKETS
                                                  , eSnapshotState::Initial );
        }
    }
}

void
FOdysseyVectorUndoSelectVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotBuffer )
    {
        pathSnapshot.LoadState( eSnapshotState::Altered );
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotBuffer )
    {
        paintgroupSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoSelectVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotBuffer )
    {
        pathSnapshot.RecordState( eSnapshotState::Altered );
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotBuffer )
    {
        paintgroupSnapshot.RecordState( eSnapshotState::Altered );
    }


    for( FSnapshotPath& pathSnapshot : mPathSnapshotBuffer )
    {
        pathSnapshot.LoadState( eSnapshotState::Initial );
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotBuffer )
    {
        paintgroupSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectVertex::ToString() const
{
    return FString("FOdysseyVectorUndoSelectVertex");
}
