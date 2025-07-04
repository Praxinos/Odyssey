// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoObjectTransform::~FOdysseyVectorUndoObjectTransform()
{
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , FOdysseyVectorObject* iObject
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mObjectSnapshotBuffer.emplace_back( iObject
                                      , FSnapshotFlags::Object::TRANSFORMATIONS
                                      , eSnapshotState::Initial );
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , std::list<FOdysseyVectorObject*>& iObjectList
                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mObjectSnapshotBuffer.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        mObjectSnapshotBuffer.emplace_back( vectorObject
                                          , FSnapshotFlags::Object::TRANSFORMATIONS
                                          , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotObject& objectSnapshot : mObjectSnapshotBuffer )
    {
        objectSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotObject& objectSnapshot : mObjectSnapshotBuffer )
    {
        objectSnapshot.RecordState( eSnapshotState::Altered );
    }


    for( FSnapshotObject& objectSnapshot : mObjectSnapshotBuffer )
    {
        objectSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
