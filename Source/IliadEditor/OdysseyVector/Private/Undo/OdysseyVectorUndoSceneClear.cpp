// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Undo/OdysseyVectorUndoSceneClear.h"

#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyVectorUndoSceneClear::~FOdysseyVectorUndoSceneClear()
{
    delete mScene;
}

FOdysseyVectorUndoSceneClear::FOdysseyVectorUndoSceneClear( FOdysseyVectorGroupPaint* iScene
                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mScene ( iScene )
    , mCell ( iScene->GetCell() )
{

}

void
FOdysseyVectorUndoSceneClear::Apply( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mCell->GetScene();

    FOdysseyVectorUndo::Apply( iIgnored );

    mCell->SetScene( mScene );

    mScene = savedScene;

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoSceneClear::Revert( UObject* iIgnored )
{
    FOdysseyVectorGroupPaint* savedScene = mCell->GetScene();

    FOdysseyVectorUndo::Revert( iIgnored );

    mCell->SetScene( mScene );

    mScene = savedScene;

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSceneClear::ToString() const
{
    return FString("FOdysseyVectorUndoSceneClear");
}
