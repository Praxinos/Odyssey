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

    // Updates the selection box
    FOdysseyPainterEditorVectorBaseToolHUD::Reset( iScene );
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

void
FOdysseyPainterEditorVectorPathCutToolHUD::Draw( BLContext* iBLContext
                                               , FOdysseyVectorScene* iScene
                                               , uint64 iDrawingFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    // Draw scene in object or vertex mode
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene, iDrawingFlags );

    iBLContext->save();

    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeLine( mPoint[0].x, mPoint[0].y, mPoint[1].x, mPoint[1].y );

    iBLContext->restore();
}
