#include "Undo/OdysseyVectorUndoObjectAdd.h"

FOdysseyVectorUndoObjectAdd::~FOdysseyVectorUndoObjectAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        for( int i = 0; i < mObjectArray.size(); i++ )
        {
            delete mObjectArray[i];
        }
    }
}

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorScene* iScene
                                                        , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
{
    mFormerParentArray.push_back( iObject->GetParent() );
    mObjectArray.push_back( iObject );
}

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorScene* iScene
                                                        , std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = iObjectList.begin(); it != iObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mFormerParentArray.push_back( object->GetParent() );
        mObjectArray.push_back( object );
    }
}

void
FOdysseyVectorUndoObjectAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mObjectArray.size(); i++ )
    {
        FOdysseyVectorObject* currentParent = mObjectArray[i]->GetParent();

        if( currentParent )
        {
            mFormerParentArray[i]->TransferChild( mObjectArray[i] );
        }
        else
        {
            mFormerParentArray[i]->AppendChild( mObjectArray[i] );
        }

        mFormerParentArray[i] = currentParent;
    }

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED
                  | FOdysseyVectorScene::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoObjectAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mObjectArray.size(); i++ )
    {
        FOdysseyVectorObject* currentParent = mObjectArray[i]->GetParent();

        if( mFormerParentArray[i] )
        {
            mFormerParentArray[i]->TransferChild( mObjectArray[i] );
        }
        else
        {
            currentParent->RemoveChild( mObjectArray[i] );

            mObjectArray[i]->SetParent( nullptr );
        }

        mFormerParentArray[i] = currentParent;
    }

    mScene->ClearSelection();

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
FOdysseyVectorUndoObjectAdd::ToString() const
{
    return FString("FOdysseyVectorUndoObjectAdd");
}
