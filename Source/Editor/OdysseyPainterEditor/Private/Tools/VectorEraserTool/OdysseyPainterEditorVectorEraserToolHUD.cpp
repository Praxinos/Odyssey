#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorEraserToolHUD::~FOdysseyPainterEditorVectorEraserToolHUD()
{

}

FOdysseyPainterEditorVectorEraserToolHUD::FOdysseyPainterEditorVectorEraserToolHUD( UOdysseyPainterEditorVectorEraserTool* iEraserTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iEraserTool )
{
    mEraserTool = iEraserTool;
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Load( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetPreferredWidth();
    uint32 height = vectorEngine->GetPreferredHeight();

    mBLEraserMask.create( width, height, BL_FORMAT_A8 );

    mBLEraserContext.begin( mBLEraserMask );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Unload( FOdysseyVectorScene* iScene )
{
    mBLEraserContext.end();
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    ClearMask();

    // Updates the selection box
    FOdysseyPainterEditorVectorBaseToolHUD::Reset( iScene );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Draw( BLContext* iBLContext
                                              , FOdysseyVectorScene* iScene
                                              , uint64 iDrawingFlags )
{
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    BLPath path;
    BLPoint topLeft = { 0, 0 };

    // Draw scene in object or vertex mode
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene, iDrawingFlags );

    mBLEraserContext.flush( BL_CONTEXT_FLUSH_SYNC );

    if( mBlending == true )
    {
        iBLContext->blitImage( topLeft, mBLEraserMask );
    }

    iBLContext->save();

    iBLContext->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mEraserTool->Radius );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorEraserToolHUD::FillCircle( double iX, double iY )
{
    mBLEraserContext.setFillAlpha( 1.0f );
    mBLEraserContext.fillCircle( iX, iY, mEraserTool->Radius );
}

void
FOdysseyPainterEditorVectorEraserToolHUD::StrokeLine( const ::ULIS::FVec2D& iP0, const ::ULIS::FVec2D& iP1 )
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
