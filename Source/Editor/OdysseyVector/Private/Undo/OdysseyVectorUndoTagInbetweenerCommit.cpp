#include "Undo/OdysseyVectorUndoTagInbetweenerCommit.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerCommit::~FOdysseyVectorUndoTagInbetweenerCommit()
{
    if( mApplied )
    {
        for( FOdysseyVectorTag* tag : mRemovedTagList )
        {
            delete tag;
        }
    }
    else
    {
        for( FOdysseyVectorObject* object : mAddedObjectList )
        {
            delete object;
        }
    }
}

FOdysseyVectorUndoTagInbetweenerCommit::FOdysseyVectorUndoTagInbetweenerCommit( FOdysseyVectorGroupPaint* iScene
                                                                              , const std::list<FOdysseyVectorTag*>& iRemovedTagList 
                                                                              , const std::list<FOdysseyVectorObject*>& iAddedObjectList
                                                                              , const std::list<FOdysseyVectorGroupPaint*>& iCommittedSceneList
                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mRemovedTagList( iRemovedTagList )
    , mAddedObjectList( iAddedObjectList )
    , mCommittedSceneList( iCommittedSceneList )
{
}

void
FOdysseyVectorUndoTagInbetweenerCommit::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorTag* tag : mRemovedTagList )
    {
        tag->GetOwner()->RemoveTag( tag );
    }

    for( FOdysseyVectorObject* object : mAddedObjectList )
    {
        object->GetOldParent()->AppendChild( object );
    }

    for( FOdysseyVectorGroupPaint* scene : mCommittedSceneList )
    {
        //will update paintgroup's mPathList e.
        scene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        scene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerCommit::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorTag* tag : mRemovedTagList )
    {
        tag->GetOwner()->AddTag( tag );
    }

    for( FOdysseyVectorObject* object : mAddedObjectList )
    {
        object->GetParent()->RemoveChild( object );
    }

    for( FOdysseyVectorGroupPaint* scene : mCommittedSceneList )
    {
        //will update paintgroup's mPathList e.g
        scene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        scene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerCommit::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerCommit");
}
