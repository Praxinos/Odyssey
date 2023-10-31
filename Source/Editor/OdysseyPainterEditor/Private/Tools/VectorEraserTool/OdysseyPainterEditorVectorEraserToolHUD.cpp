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

    UpdateSelectionBox( iScene, mEraserTool->GetEditor()->GetVectorEditionFlags() );
}

// tells in which case an object is displayed by the tool
bool
FOdysseyPainterEditorVectorEraserToolHUD::IsObjectDisplayed( FOdysseyVectorScene* iScene
                                                           , FOdysseyVectorObject* iObject
                                                           , uint64 iHUDFlags )
{
    if ( iHUDFlags & VIEW_MODE_VERTEX )
    {
        if( mEraserTool->RestrictToSelection == false )
        {
            return true;
        }

        if( ( mEraserTool->RestrictToSelection == true ) && iObject->IsSelected() )
        {
            return true;
        }
    }

    return false;
}

// tells in which case an object is altered by the tool
bool
FOdysseyPainterEditorVectorEraserToolHUD::IsObjectAltered( FOdysseyVectorScene* iScene
                                                         , FOdysseyVectorObject* iObject
                                                         , uint64 iHUDFlags )
{
    if( mEraserTool->RestrictToSelection == false )
    {
        return true;
    }

    if( ( mEraserTool->RestrictToSelection == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
}

void
FOdysseyPainterEditorVectorEraserToolHUD::Draw( BLContext* iBLContext
                                              , FOdysseyVectorScene* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mEraserTool->GetEditor()->GetVectorEditionFlags();

    // Draw object details only in vertex mode
    if( hudFlags & VIEW_MODE_VERTEX )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , iScene
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags | VIEW_PATH_VERTEX | VIEW_PATH_SEGMENT );
    }

    // draw selection box only if we restrict erasure to the selection 
    if( mEraserTool->RestrictToSelection && iScene->GetSelectedObjectList().size() )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    // Prepare bliting the erasing mask
    mBLEraserContext.flush( BL_CONTEXT_FLUSH_SYNC );

    if( mBlending == true )
    {
        iBLContext->blitImage( BLPoint( 0, 0 ), mBLEraserMask );
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
