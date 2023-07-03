#include "Undo/OdysseyVectorUndoEngineClear.h"

FOdysseyVectorUndoEngineClear::~FOdysseyVectorUndoEngineClear()
{
    delete mScene;
}

FOdysseyVectorUndoEngineClear::FOdysseyVectorUndoEngineClear( FOdysseyVectorEngine* iEngine )
    : FOdysseyVectorUndo( iEngine->GetScene() )
    , mEngine ( iEngine )
{
}

void
FOdysseyVectorUndoEngineClear::Apply( UObject* iIgnored )
{
    FOdysseyVectorScene* savedScene = mEngine->GetScene();

    FOdysseyVectorUndo::Apply( iIgnored );

    mEngine->SetScene( mScene );

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );

    mScene = savedScene;
}

void
FOdysseyVectorUndoEngineClear::Revert( UObject* iIgnored )
{
    FOdysseyVectorScene* savedScene = mEngine->GetScene();

    FOdysseyVectorUndo::Revert( iIgnored );

    mEngine->SetScene( mScene );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW
                  | FOdysseyVectorScene::SIGNAL_OBJECT_SELECTED );

    mScene = savedScene;
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoEngineClear::ToString() const
{
    return FString("FOdysseyVectorUndoEngineClear");
}
