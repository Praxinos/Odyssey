// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingToolHUD.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"
//
#include "SOdysseyViewport.h"
#include "FOdysseySceneViewport.h"
// for 3D HUDs
#include "CanvasTypes.h"
#include "CanvasItem.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::~FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD()
{
}

FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD( UOdysseyPainterEditorVectorPrimitiveDrawingTool* iPrimitiveDrawingTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( mPrimitiveDrawingTool )
    , mPrimitiveDrawingTool( iPrimitiveDrawingTool )
{

}

void
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::Reset()
{
    //UpdateSelectionBox( false, mScenePanTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::Load()
{
    FText shiftInfoText = LOCTEXT("vector-primitive-drawing-tool-hud-info-shift", "Uniform" );

    FormatModifierInfo( nullptr, &shiftInfoText, nullptr );

    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::Unload()
{
}

void
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams )
{
   DrawModifierInfo( iParams );
}

void
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::Draw( BLContext* iBLContext )
{
}

#undef LOCTEXT_NAMESPACE
