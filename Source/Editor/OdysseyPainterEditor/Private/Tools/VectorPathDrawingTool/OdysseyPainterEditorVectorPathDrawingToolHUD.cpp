#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorPathDrawingToolHUD::~FOdysseyPainterEditorVectorPathDrawingToolHUD()
{
}

FOdysseyPainterEditorVectorPathDrawingToolHUD::FOdysseyPainterEditorVectorPathDrawingToolHUD( UOdysseyPainterEditorVectorPathDrawingTool* iPathDrawingTool )
    : mPathDrawingTool( iPathDrawingTool )
{
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    MakePointQuadTree( iScene, false );
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FOdysseyVectorPathBuilder* pathBuilder = mPathDrawingTool->GetPathBuilder();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    //mPointQuadTree->Draw( iScene, iFlags );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( fgColor );
    blctx->strokeCircle( mX, mY, mPathDrawingTool->Radius );

    if( mPathDrawingTool->Stitch )
    {
        blctx->setStrokeStyle( hcColor );
        blctx->strokeCircle( mX, mY, mPathDrawingTool->StitchingRadius );

        if( mStitchedPointArray.size() )
        {
            FOdysseyVectorPoint* point = mStitchedPointArray[0];

            if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

                if( vertex->GetSegmentCount() == 1 )
                {
                    FOdysseyVectorPath* path = vertex->GetPath();

                    if( pathBuilder == nullptr || ( pathBuilder->GetPath() == path ) )
                    {
                        DrawPath( path
                                , hcColor
                                , bgColor
                                , hcColor
                                , true // World
                                , VIEW_SEGMENT );
                    }
                }
            }
        }
    }

    blctx->restore();
}

bool
FOdysseyPainterEditorVectorPathDrawingToolHUD::SetCursorPosition( double iX, double iY )
{
    bool needsFullRedrawing = false;

    mX = iX;
    mY = iY;

    // here we use the quadtree built buy the HUD to pick points
    if( mPathDrawingTool->Stitch && mPointQuadTree )
    {
        if( mStitchedPointArray.size() )
        {
            needsFullRedrawing = true; // tells to redraw the whole buffer (to clear the drawing of the previous picked path)
        }

        mStitchedPointArray.clear();

        PickPoints( iX, iY, mPathDrawingTool->StitchingRadius, mStitchedPointArray );

        if( mStitchedPointArray.size() )
        {
            needsFullRedrawing = true; // tells to redraw the whole buffer
        }
    }

    return needsFullRedrawing;
}
