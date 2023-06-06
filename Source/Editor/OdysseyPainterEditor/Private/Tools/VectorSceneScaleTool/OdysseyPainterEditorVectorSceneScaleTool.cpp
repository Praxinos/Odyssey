// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorSceneScaleTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorSceneScaleTool::~UOdysseyPainterEditorVectorSceneScaleTool()
{
}

UOdysseyPainterEditorVectorSceneScaleTool::UOdysseyPainterEditorVectorSceneScaleTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.SceneScaleTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorSceneScaleTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorSceneScaleTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->AddHUD( &mDummyHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();

    mDownWorldMouseX = iPointInTexture.x;
    mDownWorldMouseY = iPointInTexture.y;

    mDownLocalMouseX = localCoords.x;
    mDownLocalMouseY = localCoords.y;

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    uint32 imageWidth, imageHeight;
     double factor;

    iEngine->GetColorImageSize( &imageWidth, &imageHeight );

    factor = 1.0f + (double) iPointInTexture.deltaPosition.X / imageWidth;

    iScene->Scale( iScene->GetScalingX() * factor
                 , iScene->GetScalingY() * factor );

    iScene->UpdateMatrix();

    BLPoint worldCoords = iScene->GetWorldMatrix().mapPoint( mDownLocalMouseX, mDownLocalMouseY );

    iScene->Translate( iScene->GetTranslationX() - ( worldCoords.x - mDownWorldMouseX )
                     , iScene->GetTranslationY() - ( worldCoords.y - mDownWorldMouseY ) );

    iScene->UpdateMatrix();

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES ); // update vector scene and GUI widgets via delegates.
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    iScene->Update( 0 ); // update invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE
