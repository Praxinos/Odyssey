#include "Undo/OdysseyVectorUndoEngineClear.h"

#include "OdysseyVectorEngine.h"
#include "OdysseyVectorRoot.h"

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

    mEngine->GetRoot()->SetScene( mScene );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );

    mScene = savedScene;
}

void
FOdysseyVectorUndoEngineClear::Revert( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mEngine->GetScene();

    FOdysseyVectorUndo::Revert( iIgnored );

    mEngine->GetRoot()->SetScene( mScene );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );

    mScene = savedScene;
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoEngineClear::ToString() const
{
    return FString("FOdysseyVectorUndoEngineClear");
}
