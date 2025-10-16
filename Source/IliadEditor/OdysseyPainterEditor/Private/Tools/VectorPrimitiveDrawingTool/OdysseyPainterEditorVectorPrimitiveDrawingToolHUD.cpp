// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    : FOdysseyPainterEditorVectorBaseToolHUD( iPrimitiveDrawingTool )
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
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    if( mPrimitiveDrawingTool->GetWorkingGroup() )
    {
        if( mPrimitiveDrawingTool->GetWorkingGroup()->IsVisible(true) == false )
        {
            static FText warningText = LOCTEXT( "vector-primitive-drawing-tool-nodraw-warning"
                                              , "Cannot draw inside an invisible group" );
            static FLinearColor warningColor = FLinearColor( 1.0f, 0.5f, 0.0f );

            DrawInfo( iParams, warningText, warningColor );
        }
    }
}

void
FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD::Draw( BLContext* iBLContext )
{
}

#undef LOCTEXT_NAMESPACE
