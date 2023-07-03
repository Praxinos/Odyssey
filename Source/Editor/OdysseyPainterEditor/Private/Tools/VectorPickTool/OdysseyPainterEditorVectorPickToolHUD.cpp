#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPickToolHUD::~FOdysseyPainterEditorVectorPickToolHUD()
{
}

FOdysseyPainterEditorVectorPickToolHUD::FOdysseyPainterEditorVectorPickToolHUD( UOdysseyPainterEditorVectorPickTool* iPickTool )
    : FOdysseyVectorHUDSelection()
{
    mPickTool = iPickTool;
}

void
FOdysseyPainterEditorVectorPickToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorHUDSelection::Reset( iScene );
}

void
FOdysseyPainterEditorVectorPickToolHUD::DrawVertexSelection( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::list<FOdysseyVectorObject*>::iterator it;
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    for( it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            FOdysseyVectorHUD::DrawPath( path, fgColor, bgColor, hcColor, true, false, false );
        }
    }
}

void
FOdysseyPainterEditorVectorPickToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    switch( mPickTool->EditionMode )
    {
        case EOdysseyVectorEditionMode::Object :
            FOdysseyVectorHUDSelection::Draw( iScene, iFlags );
        break;

        case EOdysseyVectorEditionMode::Vertex :
            DrawVertexSelection( iScene, iFlags );
        break;

        default:
        break;
    }

    blctx->save();
    blctx->resetMatrix();

    DrawSelectionPolygon( blctx, fgColor, bgColor, hcColor );

    blctx->restore();
}

void
FOdysseyPainterEditorVectorPickToolHUD::DrawSelectionPolygon( BLContext* iBLContext
                                                            , BLRgba32 fgColor
                                                            , BLRgba32 bgColor
                                                            , BLRgba32 hcColor )
{
    std::vector<::ULIS::FVec2D>& pointArray = mPickTool->GetPointArray();
    BLPath path;

    iBLContext->setStrokeStyle( bgColor );
    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );

    for( int i = 0; i < pointArray.size(); i++ )
    {
        int n = ( i + 1 ) % pointArray.size();

        path.moveTo( pointArray[i].x, pointArray[i].y );
        path.lineTo( pointArray[n].x, pointArray[n].y );
    }

    iBLContext->strokePath( path );
}
