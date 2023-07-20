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
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorSceneScaleTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorSceneScaleTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorSceneScaleTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->RemoveHUD( mSceneScaleHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorSceneScaleTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    //iEngine->ClearHUD();
    //iEngine->AddHUD( mSceneScaleHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
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

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
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
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Commit()
{

}

#undef LOCTEXT_NAMESPACE
