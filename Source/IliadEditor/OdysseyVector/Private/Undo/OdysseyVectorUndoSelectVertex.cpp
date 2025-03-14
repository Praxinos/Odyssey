// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    mPathSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mPathSnapshotArray.emplace_back( path, FSnapshotFlags::Object::Path::SELECTED_VERTICES );
        }

        if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mPaintgroupSnapshotArray.emplace_back( paintGroup, FSnapshotFlags::Object::GroupPaint::SELECTED_BUCKETS );
        }
    }
}

void
FOdysseyVectorUndoSelectVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotArray )
    {
        paintgroupSnapshot.Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoSelectVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    for( FSnapshotGroupPaint& paintgroupSnapshot : mPaintgroupSnapshotArray )
    {
        paintgroupSnapshot.Restore();
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
