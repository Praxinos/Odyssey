#include "Undo/OdysseyVectorUndoObjectTransform.h"

FOdysseyVectorUndoObjectTransform::~FOdysseyVectorUndoObjectTransform()
{
    objectTransformAfterArray.clear();
    objectTransformBeforeArray.clear();
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo()
    , mScene ( iScene )
{
}

void
RestoreObjectTransform( std::vector<FObjectTransform>& objectTransformArray )
{
    for( int i = 0; i < objectTransformArray.size(); i++ )
    {
        objectTransformArray[i].object->SetTransform( objectTransformArray[i].translationX
                                                    , objectTransformArray[i].translationY
                                                    , objectTransformArray[i].rotation
                                                    , objectTransformArray[i].scalingX
                                                    , objectTransformArray[i].scalingY );

        objectTransformArray[i].object->UpdateMatrix();
    }
}

static void
RecordObjectTransform( std::vector<FObjectTransform>& objectTransformArray, FOdysseyVectorObject* iObject )
{
    objectTransformArray.push_back( FObjectTransform( iObject ) );
}

void
FOdysseyVectorUndoObjectTransform::RecordTransformBefore( FOdysseyVectorObject* iObject )
{
    RecordObjectTransform( objectTransformBeforeArray, iObject );
}

void
FOdysseyVectorUndoObjectTransform::RecordTransformBefore( std::list<FOdysseyVectorObject*>& iObjectList )
{
    objectTransformBeforeArray.reserve( iObjectList.size() );

    for( std::list<FOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = (*it);

        RecordObjectTransform( objectTransformBeforeArray, object );
    }
}

void
FOdysseyVectorUndoObjectTransform::RecordTransformAfter( FOdysseyVectorObject* iObject )
{
    RecordObjectTransform( objectTransformAfterArray, iObject );
}

void
FOdysseyVectorUndoObjectTransform::RecordTransformAfter( std::list<FOdysseyVectorObject*>& iObjectList )
{
    objectTransformAfterArray.reserve( iObjectList.size() );

    for( std::list<FOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = (*it);

        RecordObjectTransform( objectTransformAfterArray, object );
    }
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    RestoreObjectTransform( objectTransformAfterArray );

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    RestoreObjectTransform( objectTransformBeforeArray );

    mScene->Update(0);

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
