#include "Undo/OdysseyVectorUndoSelectVertex.h"

FOdysseyVectorUndoSelectVertex::~FOdysseyVectorUndoSelectVertex()
{
}

FOdysseyVectorUndoSelectVertex::FOdysseyVectorUndoSelectVertex( FOdysseyVectorScene* iScene
                                                             , std::vector<FOdysseyVectorVertex*>& iPickedVertexArray
                                                             , std::vector<FOdysseyVectorBucket*>& iPickedBucketArray )
    : FOdysseyVectorUndo( iScene )
    , mPickedVertexArray( iPickedVertexArray )
    , mPickedBucketArray( iPickedBucketArray )
{
}

void
FOdysseyVectorUndoSelectVertex::Apply( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FOdysseyVectorVertex* vertex : mPickedVertexArray )
    {
        vertex->GetPath()->SelectVertex( vertex );
    }

    for( FOdysseyVectorBucket* bucket : mPickedBucketArray )
    {
        FOdysseyVectorObject* ownerObject = bucket->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        { 
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->SelectBucket( bucket );
        }
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

void
FOdysseyVectorUndoSelectVertex::Revert( UObject* iIgnored )
{
    // save former selection
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FOdysseyVectorVertex* vertex : mPickedVertexArray )
    {
        vertex->GetPath()->UnselectVertex( vertex );
    }

    for( FOdysseyVectorBucket* bucket : mPickedBucketArray )
    {
        FOdysseyVectorObject* ownerObject = bucket->GetOwner();

        if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        { 
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

            paintGroup->UnselectBucket( bucket );
        }
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelectVertex::ToString() const
{
    return FString("FOdysseyVectorUndoSelectVertex");
}
