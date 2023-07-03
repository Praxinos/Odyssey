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

// static
void
FObjectTransform::MakeArrayFromObjectList( std::list<FOdysseyVectorObject*>& iObjectList
                                         , std::vector<FObjectTransform>& oObjectTransformArray )
{
    oObjectTransformArray.reserve( iObjectList.size() );

    for( std::list<FOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = (*it);

        oObjectTransformArray.push_back( FObjectTransform( object ) );
    }
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene
                                                                    , const FObjectTransform& iObjectTransform )
    : FOdysseyVectorUndo( iScene )
{
    objectTransformArray.push_back( iObjectTransform );
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene
                                                                    , std::vector<FObjectTransform>& iObjectTransformArray )
    : FOdysseyVectorUndo( iScene )
{
    objectTransformArray = iObjectTransformArray;
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    RestoreObjectTransform( objectTransformArray );

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    RestoreObjectTransform( objectTransformArray );

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
