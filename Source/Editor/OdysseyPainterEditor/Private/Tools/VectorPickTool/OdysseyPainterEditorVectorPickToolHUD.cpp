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
            DrawSelectionSpace( iScene, iFlags );
            DrawVertexSelection( iScene, iFlags );
        break;

        default:
        break;
    }

    blctx->save();
    blctx->resetMatrix();

    DrawPickingArea( blctx, fgColor, bgColor, hcColor );

    blctx->restore();
}

void
FOdysseyPainterEditorVectorPickToolHUD::DrawPickingArea( BLContext* iBLContext
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

    if( pointArray.size() > 1 )
    {
        switch( mPickTool->GetPickingMode() )
        {
            case EOdysseyVectorPickingMode::Rectangle :
            {
                double xmin = ::ULIS::FMath::Min( pointArray[0].x, pointArray[1].x );
                double ymin = ::ULIS::FMath::Min( pointArray[0].y, pointArray[1].y );
                double xmax = ::ULIS::FMath::Max( pointArray[0].x, pointArray[1].x );
                double ymax = ::ULIS::FMath::Max( pointArray[0].y, pointArray[1].y );
                ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

                iBLContext->strokeRect( rect.x, rect.y, rect.w, rect.h );
            }
            break;

            case EOdysseyVectorPickingMode::Circle:
            {
                double xmin = ::ULIS::FMath::Min( pointArray[0].x, pointArray[1].x );
                double ymin = ::ULIS::FMath::Min( pointArray[0].y, pointArray[1].y );
                double xmax = ::ULIS::FMath::Max( pointArray[0].x, pointArray[1].x );
                double ymax = ::ULIS::FMath::Max( pointArray[0].y, pointArray[1].y );
                ::ULIS::FVec2D diagonal = ::ULIS::FVec2D( xmax, ymax ) - ::ULIS::FVec2D( xmin, ymin );
                double radius = diagonal.Distance();

                iBLContext->strokeCircle( pointArray[0].x, pointArray[0].y, radius );
            }
            break;

            case EOdysseyVectorPickingMode::Freehand : 
                for( int i = 0; i < pointArray.size(); i++ )
                {
                    int n = ( i + 1 ) % pointArray.size();

                    path.moveTo( pointArray[i].x, pointArray[i].y );
                    path.lineTo( pointArray[n].x, pointArray[n].y );
                }

                iBLContext->strokePath( path );
            break;

            default:

            break;
        }
    }
}
