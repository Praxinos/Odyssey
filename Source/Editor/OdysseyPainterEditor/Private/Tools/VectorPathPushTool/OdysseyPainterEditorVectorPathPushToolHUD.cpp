#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathPushToolHUD::~FOdysseyPainterEditorVectorPathPushToolHUD()
{
}

FOdysseyPainterEditorVectorPathPushToolHUD::FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathPushTool )
    , mPathPushTool( iPathPushTool )
    , mX( 0.0f )
    , mY( 0.0f )
{

}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    uint64 hudFlags = GetViewingMode();

    // Updates the selection box
    UpdateSelectionBox( iScene, hudFlags );
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Load(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = GetViewingMode();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

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

    // draw selection box only if we restrict pushing to the selection 
    if( mPathPushTool->RestrictToSelectedObjects )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    iBLContext->save();

    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeCircle( mX, mY, mPathPushTool->Radius );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::SetCursorPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}
