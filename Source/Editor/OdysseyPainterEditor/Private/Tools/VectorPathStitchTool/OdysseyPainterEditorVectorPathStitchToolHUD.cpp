#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathStitchToolHUD::~FOdysseyPainterEditorVectorPathStitchToolHUD()
{
}

FOdysseyPainterEditorVectorPathStitchToolHUD::FOdysseyPainterEditorVectorPathStitchToolHUD( UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool )
    : mPathStitchTool( iPathStitchTool )
{
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    mPickedPointArray.clear();

    MakePointQuadTree( iScene, mPathStitchTool->RestrictToSelection );
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathStitchToolHUD::GetPickedPointArray()
{
    return mPickedPointArray;
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::SetPosition( double iWorldX, double iWorldY )
{
    mPickedPointArray.clear();

    mX = iWorldX;
    mY = iWorldY;

    PickPoints( iWorldX, iWorldY, mPathStitchTool->PickingRadius, mPickedPointArray );
}

void
FOdysseyPainterEditorVectorPathStitchToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
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
    blctx->strokeCircle( mX, mY, mPathStitchTool->PickingRadius );

    if( mPickedPointArray.size() > 1 )
    {
        FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[0]);
        FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>(mPickedPointArray[1]);

        if( ( vertex0->GetSegmentCount() == 1 ) && ( vertex1->GetSegmentCount() == 1 ) )
        {
            DrawPath( vertex0->GetPath(), fgColor, bgColor, hcColor, true, VIEW_SEGMENT );
            DrawPath( vertex1->GetPath(), fgColor, bgColor, hcColor, true, VIEW_SEGMENT );

            DrawVertex( vertex0, blctx, fgColor, bgColor, hcColor, true, 0 );
            DrawVertex( vertex1, blctx, fgColor, bgColor, hcColor, true, 0 );
        }
    }

    blctx->restore();
}
