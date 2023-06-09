// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorScenePanTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorScenePanTool::~UOdysseyPainterEditorVectorScenePanTool()
{
}

UOdysseyPainterEditorVectorScenePanTool::UOdysseyPainterEditorVectorScenePanTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ScenePanTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorScenePanTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorScenePanTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->ClearHUD();
    //iEngine->AddHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorScenePanTool","Vector Scene Pan Tool"));
    if( GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, FObjectTransform( iScene ) );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    iScene->Translate( iScene->GetTranslationX() + iPointInTexture.deltaPosition.X
                     , iScene->GetTranslationY() + iPointInTexture.deltaPosition.Y );

    iScene->UpdateMatrix();

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES ); // update vector scene and GUI widgets via delegates.
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE
