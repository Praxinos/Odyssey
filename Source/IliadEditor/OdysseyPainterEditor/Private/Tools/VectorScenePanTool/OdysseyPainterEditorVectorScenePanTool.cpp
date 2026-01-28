// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditorSource.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorScenePanTool::~UOdysseyPainterEditorVectorScenePanTool()
{
}

UOdysseyPainterEditorVectorScenePanTool::UOdysseyPainterEditorVectorScenePanTool()
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorScenePanToolHUD>( this ), false, true )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.ScenePan64"));

    mHasContextMenu = false;

    mScenePanHUD = static_cast<FOdysseyPainterEditorVectorScenePanToolHUD*>( mBaseHUD.Get() );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorScenePanTool::IsActivable() const
{
    uint64 HUDFlags = GetEditor()->GetVectorHUDFlags();

    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>()
          && ( HUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT
            || HUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX );
}

uint64
UOdysseyPainterEditorVectorScenePanTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorScenePanTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return 0;
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    BLPoint localCoords = iScene->GetInverseWorldMatrix().map_point(iPointInTexture.x,iPointInTexture.y);

    mDragged = false;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene-pan-tool.transaction.pan-scene","Pan Scene"));
    if( GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene
                                                                        , iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    mDownLocalMouseX = localCoords.x;
    mDownLocalMouseY = localCoords.y;

    // redraw
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::Pan( FOdysseyVectorGroupPaint* iScene
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
}

void
UOdysseyPainterEditorVectorScenePanTool::Scale( FOdysseyVectorGroupPaint* iScene
                                              , const FOdysseyPoint& iPointInTexture )
{
    BLPoint worldMouseCoordsBefore = iScene->GetWorldMatrix().map_point( mDownLocalMouseX, mDownLocalMouseY );
    uint32 imageWidth = iScene->GetLayer()->GetWidth();
    uint32 imageHeight = iScene->GetLayer()->GetHeight();
    double factor;

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

    BLPoint worldMouseCoordsAfter = iScene->GetWorldMatrix().map_point( mDownLocalMouseX, mDownLocalMouseY );

    iScene->Translate( iScene->GetTranslationX() - ( worldMouseCoordsAfter.x - worldMouseCoordsBefore.x )
                     , iScene->GetTranslationY() - ( worldMouseCoordsAfter.y - worldMouseCoordsBefore.y ) );

    iScene->UpdateMatrix();
}

void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    mDragged = true;

    if( iPointInTexture.keysDown.Find( EKeys::RightMouseButton ) != INDEX_NONE)
    {
        Scale( iScene, iPointInTexture );
    }
    else // Right-click has priority over left click
    {
        if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
        {
            Pan( iScene, iPointInTexture );
        }
    }

    // redraw
    //iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    return true;
}

FText
UOdysseyPainterEditorVectorScenePanTool::GetTooltip() const
{
    return LOCTEXT("vector-scene-pan-tool.tooltip", "Scene Pan Tool");
}

#undef LOCTEXT_NAMESPACE
