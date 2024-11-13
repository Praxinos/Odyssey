#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushToolHUD.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroupPaint.h"

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
FOdysseyPainterEditorVectorPathPushToolHUD::Reset(FOdysseyVectorGroupPaint* iScene)
{
    // Updates the selection box
    UpdateSelectionBox( iScene, false, mPathPushTool->GetEditor()->GetVectorHUDFlags() );
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Load(FOdysseyVectorGroupPaint* iScene)
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorPathPushToolHUD::Draw( BLContext* iBLContext
                                                , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mPathPushTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    // draw selection box only if we restrict pushing to the selection
    if( mPathPushTool->RestrictToSelectedObjects && iScene->GetEngine()->GetSelectedObjectList().size() && ( mPathPushTool->IsDragging() == false ) )
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
