// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"

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
UOdysseyPainterEditorVectorScenePanTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();

    mUndoObjectTransform = nullptr;
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , FOdysseyVectorUndo** iUndo
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        mUndoObjectTransform = new FOdysseyVectorUndoObjectTransform( iScene );
        // save selected object translation/rotation/scaling before transform
        mUndoObjectTransform->RecordTransformBefore( iScene );

        (*iUndo) = mUndoObjectTransform;

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture )
{
    iScene->Translate( iScene->GetTranslationX() + iPointInTexture.deltaPosition.X
                     , iScene->GetTranslationY() + iPointInTexture.deltaPosition.Y );

    iScene->UpdateMatrix();
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseUp( FOdysseyVectorEngine* iEngine
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
UOdysseyPainterEditorVectorScenePanTool::Commit()
{

}
