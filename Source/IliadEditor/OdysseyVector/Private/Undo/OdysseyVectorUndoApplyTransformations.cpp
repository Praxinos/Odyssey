// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoApplyTransformations.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"


FOdysseyVectorUndoApplyTransformations::~FOdysseyVectorUndoApplyTransformations()
{
    for( int i = 0 ; i < mObjectSnapshotArray.size(); i++ )
    {
        delete mObjectSnapshotArray[i];
    }
}

FOdysseyVectorUndoApplyTransformations::FOdysseyVectorUndoApplyTransformations( FOdysseyVectorGroupPaint* iScene
                                                                              , std::list<FOdysseyVectorObject*>& iObjectList
                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mObjectSnapshotArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

            mObjectSnapshotArray.push_back( new FSnapshotPath( path
                                                             , FSnapshotFlags::Object::TRANSFORMATIONS
                                                             | FSnapshotFlags::Object::Path::VERTICES
                                                             | FSnapshotFlags::Object::Path::SEGMENTS
                                                             , eSnapshotState::Initial ) );
        }
        else
        if( object->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

            mObjectSnapshotArray.push_back( new FSnapshotGroupPaint( paintGroup
                                                                   , FSnapshotFlags::Object::TRANSFORMATIONS
                                                                   //| FSnapshotFlags::Object::CHILDREN_TRANSFORMATIONS
                                                                   | FSnapshotFlags::Object::Group::Paint::BUCKETS
                                                                   , eSnapshotState::Initial ) );
        }
        else
        {
            mObjectSnapshotArray.push_back( new FSnapshotObject( object
                                                               , FSnapshotFlags::Object::TRANSFORMATIONS
                                                               , eSnapshotState::Initial ) );
        }
    }
}

void
FOdysseyVectorUndoApplyTransformations::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoApplyTransformations::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->RecordState( eSnapshotState::Altered );
    }


    for( int i = 0; i < mObjectSnapshotArray.size(); i++ )
    {
        mObjectSnapshotArray[i]->LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoApplyTransformations::ToString() const
{
    return FString("FOdysseyVectorUndoApplyTransformations");
}
