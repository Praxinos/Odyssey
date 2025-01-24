// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoDeleteVertex.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoDeleteVertex::~FOdysseyVectorUndoDeleteVertex()
{
}

FOdysseyVectorUndoDeleteVertex::FOdysseyVectorUndoDeleteVertex( FOdysseyVectorGroupPaint* iScene
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

            mPathSnapshotArray.emplace_back( path, FSnapshotFlags::Object::Path::TOPOLOGY );
        }
    }
}

void
FOdysseyVectorUndoDeleteVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoDeleteVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotPath& pathSnapshot : mPathSnapshotArray )
    {
        pathSnapshot.Restore();
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
