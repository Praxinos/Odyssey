#include "Undo/OdysseyVectorUndoObjectTransform.h"

FOdysseyVectorUndoObjectTransform::~FOdysseyVectorUndoObjectTransform()
{
    mObjectTransformArray.clear();
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

    for( FOdysseyVectorObject* object : iObjectList )
    {
        oObjectTransformArray.push_back( FObjectTransform( object ) );
    }
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
{
    mObjectTransformArray.emplace_back( iObject );
}

FOdysseyVectorUndoObjectTransform::FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                                                    , std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    FObjectTransform::MakeArrayFromObjectList( iObjectList, mObjectTransformArray );
}

void
FOdysseyVectorUndoObjectTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    RestoreObjectTransform( mObjectTransformArray );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoObjectTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    RestoreObjectTransform( mObjectTransformArray );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectTransform::ToString() const
{
    return FString("FOdysseyVectorUndoObjectTransform");
}
