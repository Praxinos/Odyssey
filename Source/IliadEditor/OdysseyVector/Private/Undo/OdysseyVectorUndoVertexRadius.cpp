// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoVertexRadius.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoVertexRadius::~FOdysseyVectorUndoVertexRadius()
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

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorPath*>& iPathArray
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mPathSnapshotArray.reserve( iPathArray.size() );

    for( int i = 0; i < iPathArray.size(); i++ )
    {
        mPathSnapshotArray.push_back( FSnapshotPath( iPathArray[i], FSnapshotFlags::Object::Path::VERTICES ));
    }
}

FOdysseyVectorUndoVertexRadius::FOdysseyVectorUndoVertexRadius( FOdysseyVectorGroupPaint* iScene
                                                              , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mVertexSnapshotArray.reserve( iVertexArray.size() );

    for( FOdysseyVectorVertex* vertex : iVertexArray )
    {
        mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotFlags::Point::Vertex::RADIUS ));
    }
}

void
FOdysseyVectorUndoVertexRadius::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mPathSnapshotArray.size(); i++ )
    {
        mPathSnapshotArray[i].Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoVertexRadius::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
    {
        mVertexSnapshotArray[i].Restore();
    }

    for( int i = 0; i < mPathSnapshotArray.size(); i++ )
    {
        mPathSnapshotArray[i].Restore();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexRadius::ToString() const
{
    return FString("FOdysseyVectorUndoVertexRadius");
}
