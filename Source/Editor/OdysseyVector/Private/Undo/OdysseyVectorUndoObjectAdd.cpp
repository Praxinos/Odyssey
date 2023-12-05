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

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorGroupPaint* iScene
                                                        , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
{
    mObjectArray.push_back( iObject );
}

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorGroupPaint* iScene
                                                        , std::list<FOdysseyVectorObject*>& iObjectList )
    : FOdysseyVectorUndo( iScene )
{
    for( FOdysseyVectorObject* object : iObjectList )
    {
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
        mParentArray[i]->AppendChild( mObjectArray[i] );
    }

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

void
FOdysseyVectorUndoObjectAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    if( mObjectArray.size() )
    {
        mParentArray.resize( mObjectArray.size() );

        for( int i = 0; i < mObjectArray.size(); i++ )
        {
            mParentArray[i] = mObjectArray[i]->GetParent();

            mParentArray[i]->RemoveChild( mObjectArray[i] );
        }
    }

    mScene->GetEngine()->ClearObjectSelection();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectAdd::ToString() const
{
    return FString("FOdysseyVectorUndoObjectAdd");
}
