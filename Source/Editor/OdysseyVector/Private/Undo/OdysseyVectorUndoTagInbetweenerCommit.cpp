#include "Undo/OdysseyVectorUndoTagInbetweenerCommit.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

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
        // request redraw attached cells
        scene->GetEngine()->Invalidate( 0 );
    }

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request radraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
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
        scene->GetEngine()->Invalidate( 0 );
    }

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request radraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerCommit::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerCommit");
}
