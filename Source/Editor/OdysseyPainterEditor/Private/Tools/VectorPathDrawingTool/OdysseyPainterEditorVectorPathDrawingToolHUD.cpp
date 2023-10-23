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
FOdysseyPainterEditorVectorPathDrawingToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathDrawingToolHUD::GetStitchedPointArray()
{
    return mStitchedPointArray;
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Draw( BLContext* iBLContext
                                                   , FOdysseyVectorScene* iScene
                                                   , uint64 iFlags )
{
    FOdysseyVectorPathTracer& pathTracer = mPathDrawingTool->GetPathTracer();
    std::vector<FTracerRecord>& recordArray = pathTracer.GetRecordArray();
    std::vector<FTracerPoint>& pointArray = pathTracer.GetPointArray();
    std::vector<FTracerEdge>& edgeArray = pathTracer.GetEdgeArray();
    FOdysseyVectorPath* path = pathTracer.GetPath();
    FTracerBezier& bestBezier = pathTracer.GetBestBezier();
    FTracerBezier& rawBezier = pathTracer.GetRawBezier();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    if( mPathDrawingTool->Stitch )
    {
        if( mStitchedPointArray.size() )
        {
            FOdysseyVectorPoint* point = mStitchedPointArray[0];

            if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

                if( vertex->GetSegmentCount() == 1 )
                {
                    FOdysseyVectorPath* stitchedPath = vertex->GetPath();

                    DrawPath( iBLContext
                            , stitchedPath
                            , hcColor
                            , bgColor
                            , hcColor
                            , true // World
                            , VIEW_SEGMENT );
                }
            }
        }
    }

    if( path )
    {
        FColor pathcolor = path->GetForegroundColor();

        iBLContext->save();
        iBLContext->resetMatrix();

        iBLContext->setStrokeStyle( BLRgba32( pathcolor.R, pathcolor.G, pathcolor.B, pathcolor.A ) );

        for( int n = 1; n < pointArray.size(); n++)
        {
            int i = n - 1;

            iBLContext->setStrokeWidth( pointArray[i].radius );
            iBLContext->strokeLine( pointArray[i].coords.x, pointArray[i].coords.y
                                  , pointArray[n].coords.x, pointArray[n].coords.y );
        }

        iBLContext->setFillStyle( BLRgba32( pathcolor.R, pathcolor.G, pathcolor.B, pathcolor.A ) );

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

            iBLContext->fillPolygon( pt, 4 );
        }

        if( mPathDrawingTool->Debug )
        {
            BLImage* mask = pathTracer.GetBLImage();

            iBLContext->blitImage( BLPoint( 0, 0 ), *mask );

            iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
            iBLContext->setStrokeWidth( 1.0f );
            BLPath rawPath;
            rawPath.moveTo( rawBezier.pt[0].x, rawBezier.pt[0].y );
            rawPath.cubicTo( rawBezier.pt[1].x, rawBezier.pt[1].y
                           , rawBezier.pt[2].x, rawBezier.pt[2].y
                           , rawBezier.pt[3].x, rawBezier.pt[3].y );
            iBLContext->strokePath( rawPath );

            iBLContext->setStrokeStyle( BLRgba32( 0, 255, 0, 255 ) );
            iBLContext->setStrokeWidth( 1.0f );
            BLPath bestPath;
            bestPath.moveTo( bestBezier.pt[0].x, bestBezier.pt[0].y );
            bestPath.cubicTo( bestBezier.pt[1].x, bestBezier.pt[1].y
                            , bestBezier.pt[2].x, bestBezier.pt[2].y
                            , bestBezier.pt[3].x, bestBezier.pt[3].y );
            iBLContext->strokePath( bestPath );
        }

        iBLContext->restore();
    }
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
