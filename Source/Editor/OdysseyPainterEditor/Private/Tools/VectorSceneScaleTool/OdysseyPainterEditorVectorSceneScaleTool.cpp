// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorSceneScaleTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorSceneScaleTool::~UOdysseyPainterEditorVectorSceneScaleTool()
{
}

UOdysseyPainterEditorVectorSceneScaleTool::UOdysseyPainterEditorVectorSceneScaleTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.SceneScaleTool64");

    //mSceneScaleHUD = new FOdysseyPainterEditorVectorScenePanToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorSceneScaleTool::Load()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorSceneScaleTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Unload()
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorSceneScaleTool::UnloadVector( vectorEngine, vectorScene );
    }
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::IsActivable() const
{
    return !!mToolContext->GetVectorEngine();
}

void
UOdysseyPainterEditorVectorSceneScaleTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( mSceneScaleHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorSceneScaleTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->ClearHUD();
    //iEngine->AddHUD( mSceneScaleHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    }
    return ret;
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorSceneScaleTool","Vector Scene Scale Tool"));
    if( GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, FObjectTransform( iScene ) );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    mDownLocalMouseX = localCoords.x;
    mDownLocalMouseY = localCoords.y;

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

//static
void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDragVectorStatic( double iLocalMouseAtDownX
                                                                  , double iLocalMouseAtDownY
                                                                  , FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorScene* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    uint32 imageWidth, imageHeight;
    double factor;
    BLPoint worldMouseCoordsBefore = iScene->GetWorldMatrix().mapPoint( iLocalMouseAtDownX, iLocalMouseAtDownY );

    iEngine->GetColorImageSize( &imageWidth, &imageHeight );

    factor = (double) iPointInTexture.deltaPosition.X / imageWidth;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        factor *= 4.0f;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        factor *= 0.25f;
    }

    iScene->Scale( iScene->GetScalingX() * ( 1.0f + factor )
                 , iScene->GetScalingY() * ( 1.0f + factor ) );

    iScene->UpdateMatrix();

    BLPoint worldMouseCoordsAfter = iScene->GetWorldMatrix().mapPoint( iLocalMouseAtDownX, iLocalMouseAtDownY );

    iScene->Translate( iScene->GetTranslationX() - ( worldMouseCoordsAfter.x - worldMouseCoordsBefore.x )
                     , iScene->GetTranslationY() - ( worldMouseCoordsAfter.y - worldMouseCoordsBefore.y ) );

    iScene->UpdateMatrix();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    BLPoint worldMouseCoordsBefore = iScene->GetWorldMatrix().mapPoint( mDownLocalMouseX
                                                                      , mDownLocalMouseY );

    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        OnMouseDragVectorStatic( mDownLocalMouseX
                               , mDownLocalMouseY
                               , iEngine
                               , iScene
                               , iPointInTexture );
    }

    if( iPointInTexture.keysDown.Find( EKeys::RightMouseButton ) != INDEX_NONE )
    {
        UOdysseyPainterEditorVectorScenePanTool::OnMouseDragVectorStatic( iEngine
                                                                        , iScene
                                                                        , iPointInTexture );
    }
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool ret = false;
    FOdysseyVectorEngine* vectorEngine = mToolContext->GetVectorEngine();
    if( vectorEngine )
    {
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
        ret = UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE
