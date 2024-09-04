#include "Undo/OdysseyVectorUndoEngineClear.h"

#include "OdysseyVectorEngine.h"

FOdysseyVectorUndoEngineClear::~FOdysseyVectorUndoEngineClear()
{
    delete mScene;
}

FOdysseyVectorUndoEngineClear::FOdysseyVectorUndoEngineClear( FOdysseyVectorEngine* iEngine
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iEngine->GetScene(), iReturnFlags )
    , mEngine ( iEngine )
{
}

void
FOdysseyVectorUndoEngineClear::Apply( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mEngine->GetScene();

    FOdysseyVectorUndo::Apply( iIgnored );

    mEngine->SetScene( mScene );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );

    mScene = savedScene;
}

void
FOdysseyVectorUndoEngineClear::Revert( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mEngine->GetScene();

    FOdysseyVectorUndo::Revert( iIgnored );

    mEngine->SetScene( mScene );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );

    mScene = savedScene;
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoEngineClear::ToString() const
{
    return FString("FOdysseyVectorUndoEngineClear");
}
