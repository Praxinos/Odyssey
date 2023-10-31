#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathCutToolHUD::~FOdysseyPainterEditorVectorPathCutToolHUD()
{
}

FOdysseyPainterEditorVectorPathCutToolHUD::FOdysseyPainterEditorVectorPathCutToolHUD( UOdysseyPainterEditorVectorPathCutTool* iPathCutTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathCutTool )
    , mPathCutTool( iPathCutTool )
{
    SetP0( 0.0f, 0.0f );
    SetP1( 0.0f, 0.0f );
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    SetP0( 0.0f, 0.0f );
    SetP1( 0.0f, 0.0f );

    UpdateSelectionBox( iScene, mPathCutTool->GetEditor()->GetVectorEditionFlags() );
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::SetP0( double iX, double iY )
{
    mPoint[0].x = iX;
    mPoint[0].y = iY;
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::SetP1(  double iX, double iY )
{
    mPoint[1].x = iX;
    mPoint[1].y = iY;
}

::ULIS::FVec2D&
FOdysseyPainterEditorVectorPathCutToolHUD::GetP0()
{
    return mPoint[0];
}

::ULIS::FVec2D&
FOdysseyPainterEditorVectorPathCutToolHUD::GetP1()
{
    return mPoint[1];
}

// tells in which case an object is displayed by the tool
bool
FOdysseyPainterEditorVectorPathCutToolHUD::IsObjectDisplayed( FOdysseyVectorScene* iScene
                                                            , FOdysseyVectorObject* iObject
                                                            , uint64 iHUDFlags )
{
    if( mPathCutTool->RestrictToSelectedObjects == false )
    {
        return true;
    }

    if( ( mPathCutTool->RestrictToSelectedObjects == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
}

// tells in which case an object is altered by the tool
bool
FOdysseyPainterEditorVectorPathCutToolHUD::IsObjectAltered( FOdysseyVectorScene* iScene
                                                          , FOdysseyVectorObject* iObject
                                                          , uint64 iHUDFlags )
{
    if( mPathCutTool->RestrictToSelectedObjects == false )
    {
        return true;
    }

    if( ( mPathCutTool->RestrictToSelectedObjects == true ) && iObject->IsSelected() )
    {
        return true;
    }

    return false;
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::Draw( BLContext* iBLContext
                                               , FOdysseyVectorScene* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    //static BLRgba32 whiteColor = BLRgba32( 255, 255, 255, 255 );
    uint64 hudFlags = mPathCutTool->GetEditor()->GetVectorEditionFlags();

    // Draw object details only in vertex mode
    if( hudFlags & VIEW_MODE_VERTEX )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | VIEW_PATH_VERTEX | VIEW_PATH_SEGMENT );
    }

    // draw only white vertices in object mode, to view were the cutting is going to be
    if( hudFlags & VIEW_MODE_OBJECT )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , fgColor // in object mode, we don't show the selected vertices with a different color
                   , hudFlags | VIEW_PATH_VERTEX | VIEW_PATH_SEGMENT );
    }

    // draw selection box only if we restrict cutting to the selection 
/*
    if( mPathCutTool->RestrictToSelectedObjects )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }
*/
    iBLContext->save();

    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeLine( mPoint[0].x, mPoint[0].y, mPoint[1].x, mPoint[1].y );

    iBLContext->restore();
}
