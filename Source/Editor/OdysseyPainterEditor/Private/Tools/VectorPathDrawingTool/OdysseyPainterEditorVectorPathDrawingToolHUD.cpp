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
FOdysseyPainterEditorVectorPathDrawingToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    FOdysseyVectorPathTracer& pathTracer = mPathDrawingTool->GetPathTracer();
    std::vector<FTracerRecord>& recordArray = pathTracer.GetRecordArray();
    std::vector<FTracerPoint>& pointArray = pathTracer.GetPointArray();
    std::vector<FTracerEdge>& edgeArray = pathTracer.GetEdgeArray();
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FOdysseyVectorPath* path = pathTracer.GetPath();
    //BLImage* mask = pathTracer.GetBLImage();

    //blctx->blitImage( BLPoint( 0, 0 ), *mask );

    if( path )
    {
        FColor pathcolor = path->GetForegroundColor();

        blctx->save();
        blctx->resetMatrix();

        blctx->setStrokeStyle( BLRgba32( pathcolor.R, pathcolor.G, pathcolor.B, pathcolor.A ) );

        for( int n = 1; n < pointArray.size(); n++)
        {
            int i = n - 1;

            blctx->setStrokeWidth( pointArray[i].radius );
            blctx->strokeLine( pointArray[i].coords.x, pointArray[i].coords.y
                             , pointArray[n].coords.x, pointArray[n].coords.y );
        }

        blctx->setFillStyle( BLRgba32( pathcolor.R, pathcolor.G, pathcolor.B, pathcolor.A ) );

        for( int i = 0; i < edgeArray.size(); i++ )
        {
            int n = i + 1;
            ::ULIS::FVec2D perpendicular = ::ULIS::FVec2D( -edgeArray[i].vector.y, edgeArray[i].vector.x );
            BLPoint pt[4] = { BLPoint( recordArray[i].coords.x + ( perpendicular.x * recordArray[i].radius )
                                     , recordArray[i].coords.y + ( perpendicular.y * recordArray[i].radius ) )
                           ,  BLPoint( recordArray[i].coords.x - ( perpendicular.x * recordArray[i].radius )
                                     , recordArray[i].coords.y - ( perpendicular.y * recordArray[i].radius ) )
                           ,  BLPoint( recordArray[n].coords.x - ( perpendicular.x * recordArray[n].radius )
                                     , recordArray[n].coords.y - ( perpendicular.y * recordArray[n].radius ) )
                           ,  BLPoint( recordArray[n].coords.x + ( perpendicular.x * recordArray[n].radius )
                                     , recordArray[n].coords.y + ( perpendicular.y * recordArray[n].radius ) ) };

            blctx->fillPolygon( pt, 4 );
        }

        blctx->restore();
    }
}

#ifdef unused
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
/*
    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( hcColor );
    blctx->strokeCircle( mX, mY, mPathDrawingTool->Radius );
*/
    if( mPathDrawingTool->Stitch )
    {
/*
        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( fgColor );
        blctx->strokeCircle( mX, mY, mPathDrawingTool->StitchingRadius );
*/
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
#endif

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
