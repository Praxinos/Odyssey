#include "Undo/OdysseyVectorUndoVertexUnalign.h"

FOdysseyVectorUndoVertexUnalign::~FOdysseyVectorUndoVertexUnalign()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoVertexUnalign::FOdysseyVectorUndoVertexUnalign( FOdysseyVectorScene* iScene
                                                                , const std::vector<FOdysseyVectorVertex*>& iUnalignedVertexArray )
    : FOdysseyVectorUndo( iScene )
{
    mUnalignedVertexArray = iUnalignedVertexArray;
}

void
FOdysseyVectorUndoVertexUnalign::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( int i = 0; i < mUnalignedVertexArray.size(); i++ )
    {
        mUnalignedVertexArray[i]->SetHandleAligned( false );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoVertexUnalign::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( int i = 0; i < mUnalignedVertexArray.size(); i++ )
    {
        mUnalignedVertexArray[i]->SetHandleAligned( true );
    }

    // update invalidated objects. No need to update paintgroups as no geometry moves
    mScene->Update( 0 );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoVertexUnalign::ToString() const
{
    return FString("FOdysseyVectorUndoVertexUnalign");
}
