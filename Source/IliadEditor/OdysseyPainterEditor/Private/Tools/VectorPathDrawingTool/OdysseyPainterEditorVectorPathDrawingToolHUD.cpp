// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorPathDrawingToolHUD::~FOdysseyPainterEditorVectorPathDrawingToolHUD()
{
}

FOdysseyPainterEditorVectorPathDrawingToolHUD::FOdysseyPainterEditorVectorPathDrawingToolHUD( UOdysseyPainterEditorVectorPathDrawingTool* iPathDrawingTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iPathDrawingTool )
    , mPathDrawingTool( iPathDrawingTool )
    , mOwnerObject ( "DummyOwnerObject" ) // unused . testing
    , mVertex { FOdysseyVectorVertex( 0.0f, 0.0f, 0.0f ) // unused . testing
              , FOdysseyVectorVertex( 0.0f, 0.0f, 0.0f ) } // unused . testing
    , mCubicSegment ( &mOwnerObject, &mVertex[0], &mVertex[1], true ) // unused . testing
{
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Reset()
{
    uint64 hudFlags = mPathDrawingTool->GetEditor()->GetVectorHUDFlags();

    MakePointQuadTree( false, hudFlags );

    // Updates the selection box (it is not used in this tool but whatever)
    UpdateSelectionBox( false, hudFlags );
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Load()
{
    FOdysseyPainterEditorVectorBaseToolHUD::Load();
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Unload()
{
}

//--------------------------------------------------------------------------------------
// we cannot use the HUD for event handling because we need the pressure
// so the get back to the tool handling all hover/down/drag/up events for consistency
void
FOdysseyPainterEditorVectorPathDrawingToolHUD::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
}

bool
FOdysseyPainterEditorVectorPathDrawingToolHUD::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    return false;
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
}

bool
FOdysseyPainterEditorVectorPathDrawingToolHUD::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    return false;
}
//--------------------------------------------------------------------------------------

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

    // draw a thin line between edges for better visual displaying
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokeLine( pt[3], pt[0] );
    iBLContext->strokeLine( pt[2], pt[1] );
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams )
{
    mCurrentHUDParams = iParams;

    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    // PathTracer data
    FOdysseyVectorPathTracer& pathTracer = mPathDrawingTool->GetPathTracer();
    FOdysseyVectorPath* path = pathTracer.GetPath();
    uint64 hudFlags = mPathDrawingTool->GetEditor()->GetVectorHUDFlags();
    FVector2D hudCursor = iParams.mTextureToHUD.Execute( FVector2D( mX, mY ) );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );

    if( mPathDrawingTool->Stitch )
    {
        DrawPrimitiveCircle( iParams, hudCursor, mPathDrawingTool->StitchingRadius, hcColor, 1.0f );

        if( mStitchedPointArray.size() )
        {
            FOdysseyVectorPoint* point = mStitchedPointArray[0];

            if( point->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(point);

                if( vertex->GetSegmentCount() == 1 )
                {
                    FOdysseyVectorPath* stitchedPath = vertex->GetOwnerAsPath();

                    DrawPath( iParams
                            , stitchedPath
                            , hcColor
                            , bgColor
                            , hcColor
                            , FOdysseyVectorHUD::HUD_PATH_SEGMENT );
                }
            }
        }
    }

    if( mPathDrawingTool->GetWorkingGroup() )
    {
        if( mPathDrawingTool->GetWorkingGroup()->IsVisible(true) == false )
        {
            static FText warningText = LOCTEXT( "vector-path-drawing-tool-nodraw-warning"
                                              , "Cannot draw inside an invisible group" );
            static FLinearColor warningColor = FLinearColor( 1.0f, 0.5f, 0.0f );

            DrawInfo( iParams, warningText, warningColor );
        }
    }

    // invisible plane will get mouse events
    //DrawDummyPlane( iParams );
}

// part of this HUD is drawn onto the image
void
FOdysseyPainterEditorVectorPathDrawingToolHUD::Draw( BLContext* iBLContext )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    // PathTracer data
    FOdysseyVectorPathTracer& pathTracer = mPathDrawingTool->GetPathTracer();
    std::vector<FTracerRecord>& recordBuffer = pathTracer.GetRecordBuffer();
    std::vector<FTracerPoint>& pointBuffer = pathTracer.GetPointBuffer();
    std::vector<FTracerEdge>& edgeBuffer = pathTracer.GetEdgeBuffer();
    FOdysseyVectorPath* path = pathTracer.GetPath();
    FTracerBezier& bestBezier = pathTracer.GetBestBezier();
    FTracerBezier& rawBezier = pathTracer.GetRawBezier();
    uint64 hudFlags = mPathDrawingTool->GetEditor()->GetVectorHUDFlags();

    if( path )
    {
        FColor pathcolor = path->GetForegroundColor();

        iBLContext->save();
        iBLContext->resetMatrix();

        iBLContext->setStrokeStyle( BLRgba32( pathcolor.R, pathcolor.G, pathcolor.B, pathcolor.A ) );
        iBLContext->setStrokeStartCap(BL_STROKE_CAP_BUTT);
        iBLContext->setStrokeEndCap(BL_STROKE_CAP_BUTT);

        for( int n = 1; n < pointBuffer.size(); n++)
        {
            int i = n - 1;

            iBLContext->setStrokeWidth( pointBuffer[i].radius * 2.0f );
            iBLContext->strokeLine( pointBuffer[i].coords.x, pointBuffer[i].coords.y
                                  , pointBuffer[n].coords.x, pointBuffer[n].coords.y );
        }

        iBLContext->setFillStyle( BLRgba32( pathcolor.R, pathcolor.G, pathcolor.B, pathcolor.A ) );

        for( int i = 0; i < edgeBuffer.size(); i++ )
        {
            int p = i - 1;
            int n = i + 1;
            FTracerEdge* prevEdge = ( p >= 0               ) ? prevEdge = &edgeBuffer[p] : nullptr;
            FTracerEdge* nextEdge = ( n < edgeBuffer.size() ) ? nextEdge = &edgeBuffer[n] : nullptr;

            DrawEdge( iBLContext, prevEdge, &edgeBuffer[i], nextEdge );
        }

        iBLContext->restore();
    }
}

void
FOdysseyPainterEditorVectorPathDrawingToolHUD::SetCursorPosition( double iX, double iY )
{
    FOdysseyVectorPathTracer& pathTracer = mPathDrawingTool->GetPathTracer();
    FTracerBezier& bestBezier = pathTracer.GetBestBezier();

    FOdysseyPainterEditorVectorBaseToolHUD::SetCursorPosition( iX, iY );

    // here we use the quadtree built buy the HUD to pick points
    if( ( mPathDrawingTool->Stitch || mPathDrawingTool->Snap ) && mPointQuadTree )
    {
        mStitchedPointArray.clear();

        PickPoints( iX, iY, mPathDrawingTool->StitchingRadius, mStitchedPointArray );
    }
}

#undef LOCTEXT_NAMESPACE
