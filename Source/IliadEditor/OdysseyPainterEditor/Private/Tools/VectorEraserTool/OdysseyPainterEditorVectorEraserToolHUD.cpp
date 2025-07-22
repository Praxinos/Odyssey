// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorEraserToolHUD::~FOdysseyPainterEditorVectorEraserToolHUD()
{

}

FOdysseyPainterEditorVectorEraserToolHUD::FOdysseyPainterEditorVectorEraserToolHUD( UOdysseyPainterEditorVectorEraserTool* iEraserTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iEraserTool )
{
    mEraserTool = iEraserTool;
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Load()
{
    FText shiftInfoText = LOCTEXT("vector-eraser-tool-hud-info", "erase to intersection" );
    FText altInfoText = LOCTEXT("vector-eraser-tool-hud-info", "erase whole path");
    uint32 height = mScene->GetLayer()->GetHeight();
    uint32 width = mScene->GetLayer()->GetWidth();

    FormatModifierInfo( nullptr, &shiftInfoText, &altInfoText );

    mBLEraserMask.create( width, height, BL_FORMAT_A8 );

    mBLEraserContext.begin( mBLEraserMask );

    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Unload( )
{
    mBLEraserContext.end();
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Reset()
{
    ClearMask();

    UpdateSelectionBox( false, mEraserTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    uint64 hudFlags = mEraserTool->GetEditor()->GetVectorHUDFlags();
    FVector2D hudCursor = iParams.mTextureToHUD.Execute( FVector2D( mX, mY ) );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    DrawPrimitiveCircle( iParams
                       , hudCursor
                       , WorldVectorToHUD( ::ULIS::FVec2D( mX, mY )
                                         , ::ULIS::FVec2D( mEraserTool->Radius, 0 ) ).Distance()
                       , hcColor
                       , 1.0f );

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );

    //DrawModifierInfo( iParams );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Draw( BLContext* iBLContext )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mEraserTool->GetEditor()->GetVectorHUDFlags();

    // Prepare bliting the erasing mask
    mBLEraserContext.flush( BL_CONTEXT_FLUSH_SYNC );

    if( mBlending == true )
    {
        iBLContext->blitImage( BLPoint( 0, 0 ), mBLEraserMask );
    }
}

void
FOdysseyPainterEditorVectorEraserToolHUD::FillCircle( double iX
                                                    , double iY )
{
    mBLEraserContext.setFillAlpha( 1.0f );
    mBLEraserContext.fillCircle( iX, iY, mEraserTool->Radius );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::StrokeLine( const ::ULIS::FVec2D& iP0
                                                    , const ::ULIS::FVec2D& iP1 )
{
    mBLEraserContext.setFillAlpha( 1.0f );
    mBLEraserContext.setStrokeWidth( mEraserTool->Radius * 2 );
    mBLEraserContext.strokeLine( iP0.x, iP0.y, iP1.x, iP1.y );
    mBLEraserContext.fillCircle( iP1.x, iP1.y, mEraserTool->Radius );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::SetPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}

void
FOdysseyPainterEditorVectorEraserToolHUD::BlendMask( bool iBlending )
{
    mBlending = iBlending;
}

void
FOdysseyPainterEditorVectorEraserToolHUD::ClearMask()
{
    mBLEraserContext.save();

    mBLEraserContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLEraserContext.setFillAlpha( 0.0f );
    mBLEraserContext.clearAll();
    mBLEraserContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLEraserContext.restore();
}

BLImage*
FOdysseyPainterEditorVectorEraserToolHUD::GetMask()
{
    return &mBLEraserMask;
}

#undef LOCTEXT_NAMESPACE
