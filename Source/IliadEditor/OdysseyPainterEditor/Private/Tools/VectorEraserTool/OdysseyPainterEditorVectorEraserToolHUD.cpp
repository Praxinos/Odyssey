// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

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
    uint32 width = mScene->GetLayer()->GetWidth();
    uint32 height = mScene->GetLayer()->GetHeight();

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
FOdysseyPainterEditorVectorEraserToolHUD::DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams )
{
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

    DrawPrimitiveCircle( iParams, hudCursor, mEraserTool->Radius, hcColor, bgColor, 1.0f, false );
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
                                                    , double iY
                                                    , double iZoomFactor )
{
    mBLEraserContext.setFillAlpha( 1.0f );
    mBLEraserContext.fillCircle( iX, iY, mEraserTool->Radius / iZoomFactor );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::StrokeLine( const ::ULIS::FVec2D& iP0
                                                    , const ::ULIS::FVec2D& iP1
                                                    , double iZoomFactor )
{
    mBLEraserContext.setFillAlpha( 1.0f );
    mBLEraserContext.setStrokeWidth( mEraserTool->Radius * 2 );
    mBLEraserContext.strokeLine( iP0.x, iP0.y, iP1.x, iP1.y );
    mBLEraserContext.fillCircle( iP1.x, iP1.y, mEraserTool->Radius / iZoomFactor );
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
