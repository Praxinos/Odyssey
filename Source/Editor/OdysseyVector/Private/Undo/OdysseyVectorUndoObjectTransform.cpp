#include "Undo/OdysseyVectorUndoObjectTransform.h"

FOdysseyVectorUndoObjectTransform::~FOdysseyVectorUndoObjectTransform()
{
    objectTransformArray.clear();
}

static void
RestoreObjectTransform( std::vector<FObjectTransform>& objectTransformArray )
{
    for( int i = 0; i < objectTransformArray.size(); i++ )
    {
        FObjectTransform formerTransform = FObjectTransform( objectTransformArray[i].object );

        objectTransformArray[i].object->SetTransform( objectTransformArray[i].translationX
                                                    , objectTransformArray[i].translationY
                                                    , objectTransformArray[i].rotation
                                                    , objectTransformArray[i].scalingX
                                                    , objectTransformArray[i].scalingY );

        objectTransformArray[i].object->UpdateMatrix();
        // Replace stored data with former transform  (prepare for the counterpart operation, either Apply or Revert)
        objectTransformArray[i] = formerTransform;
    }
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene
                                                                    , std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    objectTransformArray.reserve( iObjectList.size() );

    for( std::list<FOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = (*it);

        objectTransformArray.push_back( FObjectTransform( object ) );
    }
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene
                                                                    , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
{
    objectTransformArray.push_back( FObjectTransform( iObject ) );
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    RestoreObjectTransform( objectTransformArray );

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    RestoreObjectTransform( objectTransformArray );

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
