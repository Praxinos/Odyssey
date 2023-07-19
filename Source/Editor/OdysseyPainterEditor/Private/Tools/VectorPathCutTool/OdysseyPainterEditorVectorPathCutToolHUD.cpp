#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathCutToolHUD::~FOdysseyPainterEditorVectorPathCutToolHUD()
{
}

FOdysseyPainterEditorVectorPathCutToolHUD::FOdysseyPainterEditorVectorPathCutToolHUD( UOdysseyPainterEditorVectorPathCutTool* iPathCutTool )
    : mPathCutTool( iPathCutTool )
{
    SetP0( 0.0f, 0.0f );
    SetP1( 0.0f, 0.0f );
}

void
FOdysseyPainterEditorVectorPathCutToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    SetP0( 0.0f, 0.0f );
    SetP1( 0.0f, 0.0f );
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
FOdysseyPainterEditorVectorPathCutToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>::iterator it;
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    blctx->save();
    blctx->resetMatrix();

    blctx->setCompOp( BL_COMP_OP_SRC_COPY );
    blctx->setStrokeStyle( hcColor );
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeLine( mPoint[0].x, mPoint[0].y, mPoint[1].x, mPoint[1].y );

    for( it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            FOdysseyVectorHUD::DrawPath( path, fgColor, bgColor, hcColor, true, VIEW_VERTEX | VIEW_SEGMENT );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            FOdysseyVectorHUD::DrawPaintGroup( paintGroup, fgColor, bgColor, hcColor, true, VIEW_VERTEX | VIEW_SEGMENT );
        }
    }

    blctx->restore();
}
