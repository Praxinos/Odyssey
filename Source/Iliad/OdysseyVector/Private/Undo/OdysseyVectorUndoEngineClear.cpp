// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoEngineClear.h"

#include "OdysseyVectorEngine.h"
#include "OdysseyVectorRoot.h"

FOdysseyVectorUndoEngineClear::~FOdysseyVectorUndoEngineClear()
{
    delete mScene;
}

FOdysseyVectorUndoEngineClear::FOdysseyVectorUndoEngineClear( FOdysseyVectorEngine* iEngine
                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iEngine->GetScene()->GetSharedEnv(), iReturnFlags )
    , mScene ( iEngine->GetScene() )
{
    mEngineList.push_back( iEngine );
}

void
FOdysseyVectorUndoEngineClear::Apply( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mEngineList.front()->GetScene();

    FOdysseyVectorUndo::Apply( iIgnored );

    mEngineList.front()->GetRoot()->SetScene( mScene );

    mScene = savedScene;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoEngineClear::Revert( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mEngineList.front()->GetScene();

    FOdysseyVectorUndo::Revert( iIgnored );

    mEngineList.front()->GetRoot()->SetScene( mScene );

    mScene = savedScene;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoEngineClear::ToString() const
{
    return FString("FOdysseyVectorUndoEngineClear");
}
