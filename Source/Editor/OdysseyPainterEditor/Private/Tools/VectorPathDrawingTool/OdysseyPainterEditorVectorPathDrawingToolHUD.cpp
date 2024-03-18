#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorPathDrawingToolHUD::~FOdysseyPainterEditorVectorPathDrawingToolHUD()
{
}

FOdysseyPainterEditorVectorPathDrawingToolHUD::FOdysseyPainterEditorVectorPathDrawingToolHUD( UOdysseyPainterEditorVectorPathDrawingTool* iPathDrawingTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathDrawingTool )
    , mPathDrawingTool( iPathDrawingTool )
{
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mPathDrawingTool->GetEditor()->GetVectorHUDFlags();

    MakePointQuadTree( iScene, false, hudFlags );

    // Updates the selection box (it is not used in this tool but whatever)
    UpdateSelectionBox( iScene, false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{
}

std::vector<FOdysseyVectorPoint*>&
FOdysseyPainterEditorVectorPathDrawingToolHUD::GetStitchedPointArray()
{
    return mStitchedPointArray;
}

void 
FOdysseyPainterEditorVectorPathDrawingToolHUD::DrawEdge( BLContext* iBLContext
                                                       , FTracerEdge* iPrevEdge
                                                       , FTracerEdge* iCurrEdge
                                                       , FTracerEdge* iNextEdge )
{
    ::ULIS::FVec2D prevEdgeVector = ( iPrevEdge ) ? iPrevEdge->vector : ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D currEdgeVector = ( iCurrEdge ) ? iCurrEdge->vector : ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D nextEdgeVector = ( iNextEdge ) ? iNextEdge->vector : ::ULIS::FVec2D( 0.0f, 0.0f );
    ::ULIS::FVec2D prevEdgePerpendicularVector;
    ::ULIS::FVec2D nextEdgePerpendicularVector;
    ::ULIS::FVec2D combinedVector = iCurrEdge->vector;
    BLPoint pt[4];

    if( iPrevEdge )
    {
        combinedVector = prevEdgeVector + currEdgeVector;

        if( combinedVector.DistanceSquared() )
        {
            combinedVector.Normalize();
        }
    }

    prevEdgePerpendicularVector = ::ULIS::FVec2D( -combinedVector.y, combinedVector.x ) * iCurrEdge->radius0;

    if( iNextEdge )
    {
        combinedVector = currEdgeVector + nextEdgeVector;

        if( combinedVector.DistanceSquared() )
        {
            combinedVector.Normalize();
        }
    }

    nextEdgePerpendicularVector = ::ULIS::FVec2D( -combinedVector.y, combinedVector.x ) * iCurrEdge->radius1;

    pt[0] = BLPoint( iCurrEdge->p0.x + prevEdgePerpendicularVector.x
                   , iCurrEdge->p0.y + prevEdgePerpendicularVector.y );

    pt[1] = BLPoint( iCurrEdge->p1.x + nextEdgePerpendicularVector.x
                   , iCurrEdge->p1.y + nextEdgePerpendicularVector.y );

    pt[2] = BLPoint( iCurrEdge->p1.x - nextEdgePerpendicularVector.x
                   , iCurrEdge->p1.y - nextEdgePerpendicularVector.y );

    pt[3] = BLPoint( iCurrEdge->p0.x - prevEdgePerpendicularVector.x
                   , iCurrEdge->p0.y - prevEdgePerpendicularVector.y );

    iBLContext->fillPolygon( pt, 4 );
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Draw( BLContext* iBLContext
                                                   , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    // PathTracer data
    FOdysseyVectorPathTracer& pathTracer = mPathDrawingTool->GetPathTracer();
    std::vector<FTracerRecord>& recordArray = pathTracer.GetRecordArray();
    std::vector<FTracerPoint>& pointArray = pathTracer.GetPointArray();
    std::vector<FTracerEdge>& edgeArray = pathTracer.GetEdgeArray();
    FOdysseyVectorPath* path = pathTracer.GetPath();
    FTracerBezier& bestBezier = pathTracer.GetBestBezier();
    FTracerBezier& rawBezier = pathTracer.GetRawBezier();
    uint64 hudFlags = mPathDrawingTool->GetEditor()->GetVectorHUDFlags();

    // Draw object details only in vertex mode
    if( hudFlags & HUD_MODE_VERTEX )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_PATH_VERTEX | HUD_PATH_SEGMENT );
    }

    //DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );

    //mPointQuadTree->Draw( iBLContext, iScene, 0 );

    if( mPathDrawingTool->Stitch )
    {
        // matrix might get altered for displaying the selection rectangle of a single object. Save it.
        iBLContext->save();

        iBLContext->setStrokeStyle( hcColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokeCircle( mX, mY, mPathDrawingTool->StitchingRadius );

        iBLContext->restore();

        if( mStitchedPointArray.size() )
        {
            FOdysseyVectorPoint* point = mStitchedPointArray[0];

            if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

                if( vertex->GetSegmentCount() == 1 )
                {
                    FOdysseyVectorPath* stitchedPath = vertex->GetOwnerAsPath();

                    DrawPath( iBLContext
                            , stitchedPath
                            , hcColor
                            , bgColor
                            , hcColor
                            , true // World
                            , HUD_PATH_SEGMENT );
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
            int p = i - 1;
            int n = i + 1;
            FTracerEdge* prevEdge = ( p >= 0               ) ? prevEdge = &edgeArray[p] : nullptr;
            FTracerEdge* nextEdge = ( n < edgeArray.size() ) ? nextEdge = &edgeArray[n] : nullptr;

            DrawEdge( iBLContext, prevEdge, &edgeArray[i], nextEdge );
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
