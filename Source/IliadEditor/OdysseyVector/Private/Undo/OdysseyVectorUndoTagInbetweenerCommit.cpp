// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerCommit.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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

FOdysseyVectorUndoTagInbetweenerCommit::FOdysseyVectorUndoTagInbetweenerCommit( FOdysseyVectorLayer* iSharedEnv
                                                                              , const std::list<FOdysseyVectorTag*>& iRemovedTagList
                                                                              , const std::list<FOdysseyVectorObject*>& iAddedObjectList
                                                                              , const std::list<FOdysseyVectorGroupPaint*>& iCommittedSceneList )
    : FOdysseyVectorUndo( iSharedEnv )
    , mRemovedTagList( iRemovedTagList )
    , mAddedObjectList( iAddedObjectList )
    , mCommittedSceneList( iCommittedSceneList )
{
    //GetEngineListFromObjectList( iAddedObjectList, mEngineList );
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

/*
    for( FOdysseyVectorGroupPaint* scene : mCommittedSceneList )
    {
        // request redraw attached cells
        scene->GetEngine()->Invalidate( 0 );
    }
*/

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

/*
    for( FOdysseyVectorGroupPaint* scene : mCommittedSceneList )
    {
        scene->GetEngine()->Invalidate( 0 );
    }
*/

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerCommit::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerCommit");
}
