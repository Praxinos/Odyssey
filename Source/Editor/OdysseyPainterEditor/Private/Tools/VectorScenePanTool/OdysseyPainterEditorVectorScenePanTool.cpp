// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorScenePanTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorScenePanTool::~UOdysseyPainterEditorVectorScenePanTool()
{
}

UOdysseyPainterEditorVectorScenePanTool::UOdysseyPainterEditorVectorScenePanTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ScenePanTool64");

    mScenePanHUD = new FOdysseyPainterEditorVectorScenePanToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorScenePanTool::Load()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorScenePanTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorVectorScenePanTool::Unload()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorScenePanTool::UnloadVector( vectorEngine, vectorScene );
    }
}

bool
UOdysseyPainterEditorVectorScenePanTool::IsActivable() const
{
    return !!mToolContext->GetVectorEngine();
}

void
UOdysseyPainterEditorVectorScenePanTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mScenePanHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorScenePanTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mScenePanHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorScenePanTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    }
    return ret;
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint(iPointInTexture.x,iPointInTexture.y);

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorScenePanTool","Pan Scene"));
    if( GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, FObjectTransform( iScene ) );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }

    mDownLocalMouseX = localCoords.x;
    mDownLocalMouseY = localCoords.y;

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
    GEditor->EndTransaction();

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorScenePanTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

// static
void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDragVectorStatic( FOdysseyVectorEngine* iEngine
                                                                , FOdysseyVectorScene* iScene
                                                                , const FOdysseyPoint& iPointInTexture )
{
    double factor = 1.0f;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        factor *= 4.0f;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        factor *= 0.25f;
    }

    iScene->Translate( iScene->GetTranslationX() + ( iPointInTexture.deltaPosition.X * factor )
                     , iScene->GetTranslationY() + ( iPointInTexture.deltaPosition.Y * factor ) );
    iScene->UpdateMatrix();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{

    if(iPointInTexture.keysDown.Find(EKeys::RightMouseButton) != INDEX_NONE)
    {
        UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDragVectorStatic(mDownLocalMouseX
                                                                          ,mDownLocalMouseY
                                                                          ,iEngine
                                                                          ,iScene
                                                                          ,iPointInTexture);
    }
    else // Right-click has priority over left click
    {
        if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
        {
            OnMouseDragVectorStatic( iEngine, iScene, iPointInTexture );
        }
    }
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorScenePanTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE
