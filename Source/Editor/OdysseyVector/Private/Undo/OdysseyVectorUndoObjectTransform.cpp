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
    : FOdysseyVectorUndo()
    , mScene ( iScene )
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
    : FOdysseyVectorUndo()
    , mScene ( iScene )
{
    objectTransformArray.push_back( FObjectTransform( iObject ) );
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    RestoreObjectTransform( objectTransformArray );

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    RestoreObjectTransform( objectTransformArray );

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
