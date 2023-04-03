// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"

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
UOdysseyPainterEditorVectorSceneScaleTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD( );

    mUndoObjectTransform = nullptr;
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , FOdysseyVectorUndo** iUndo
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        mUndoObjectTransform = new FOdysseyVectorUndoObjectTransform( iScene );
        // save selected object translation/rotation/scaling before transform
        mUndoObjectTransform->RecordTransformBefore( iScene );

        (*iUndo) = mUndoObjectTransform;

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

    mDownWorldMouseX = iPointInTexture.x;
    mDownWorldMouseY = iPointInTexture.y;

    mDownLocalMouseX = localCoords.x;
    mDownLocalMouseY = localCoords.y;

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
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
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    if( mUndoObjectTransform )
    {
        // save selected object translation/rotation/scaling after transform
        mUndoObjectTransform->RecordTransformAfter( iScene );
    }

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Commit()
{

}
