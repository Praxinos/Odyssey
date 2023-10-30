#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorSelectionToolHUD::~FOdysseyPainterEditorVectorSelectionToolHUD()
{

}

FOdysseyPainterEditorVectorSelectionToolHUD::FOdysseyPainterEditorVectorSelectionToolHUD( UOdysseyPainterEditorVectorSelectionTool* iSelectionTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iSelectionTool )
    , mShowSelectionBox( true )
    , mShowSelectionIfEmpty ( false )
{
    mSelectionTool = iSelectionTool;
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Load( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    uint32 width = vectorEngine->GetPreferredWidth();
    uint32 height = vectorEngine->GetPreferredHeight();

    mBLSelectionMask.create( width, height, BL_FORMAT_A8 );

    mBLSelectionContext.begin( mBLSelectionMask );
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Unload( FOdysseyVectorScene* iScene )
{
    mBLSelectionContext.end();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = GetViewingMode();

    UpdateSelectionBox( iScene, hudFlags );
}

BLImage*
FOdysseyPainterEditorVectorSelectionToolHUD::GetMask()
{
    return &mBLSelectionMask;
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::ShowSelectionBox( bool iShowSelectionBox )
{
    mShowSelectionBox = iShowSelectionBox;
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::GetSelectedVertices( FOdysseyVectorScene* iScene
                                                                , std::vector<FOdysseyVectorPoint*>& oPointArray )
{
    // avoir to many reallocation by reserving a decent amount of memory
    oPointArray.reserve( 200 );

    Traverse( iScene
            , iScene
            , mSelectionTool->GetEditor()->GetVectorEditionFlags()
            , [ &oPointArray]( FOdysseyVectorObject* object ) -> bool
              {
                  if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                  {
                      FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(object);

                      selectedPath->GetSelectedPoints( oPointArray
                                                     , ePointSelectionFlags::Vertex
                                                     | ePointSelectionFlags::Strict
                                                     | ePointSelectionFlags::SegmentHandle );
                  }

                  if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                      selectedPaintGroup->GetSelectedPoints( oPointArray
                                                           , ePointSelectionFlags::Bucket );
                  }

                  return false; // keep traversing
              } );
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawSelectionSpace( BLContext* iBLContext
                                                               , FOdysseyVectorScene* iScene
                                                               , uint64 iFlags )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();
    BLPoint topLeft = { 0, 0 };

    mBLSelectionContext.save();

    if( iScene->GetEngine()->GetSelectionSpace() )
    {
        FOdysseyVectorGroup* selectionSpace = iScene->GetEngine()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        //iScene->GetEngine()->UseImage( mSelectionMask );

        mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );

        mBLSelectionContext.clearAll();
        mBLSelectionContext.setFillStyle( BLRgba32( 0x80FFFFFF ) );
        mBLSelectionContext.fillRect( 0, 0, mBLSelectionMask.width(), mBLSelectionMask.height() );

        mBLSelectionContext.setMatrix( worldMatrix );
        mBLSelectionContext.setFillStyle( BLRgba32( 0x00000000/*0x800000FF*/ ) );
        mBLSelectionContext.fillRect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );

        //iScene->GetEngine()->UseImage( currentImage );
        // blit with current renderer
        iBLContext->blitImage( topLeft, mBLSelectionMask );
    }

    mBLSelectionContext.restore();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawPickingArea( BLContext* iBLContext
                                                            , BLRgba32 fgColor
                                                            , BLRgba32 bgColor
                                                            , BLRgba32 hcColor )
{
    std::vector<::ULIS::FVec2D>& pointArray = mSelectionTool->GetPointArray();
    BLPath path;

    iBLContext->setStrokeStyle( bgColor );
    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->setStrokeStyle( hcColor );
    iBLContext->setStrokeWidth( 1.0f );

    if( pointArray.size() > 1 )
    {
        switch( mSelectionTool->GetSelectionShape() )
        {
            case EOdysseyVectorSelectionShape::Rectangle :
            {
                double xmin = ::ULIS::FMath::Min( pointArray[0].x, pointArray[1].x );
                double ymin = ::ULIS::FMath::Min( pointArray[0].y, pointArray[1].y );
                double xmax = ::ULIS::FMath::Max( pointArray[0].x, pointArray[1].x );
                double ymax = ::ULIS::FMath::Max( pointArray[0].y, pointArray[1].y );
                ::ULIS::FRectD rect = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

                iBLContext->strokeRect( rect.x, rect.y, rect.w, rect.h );
            }
            break;

            case EOdysseyVectorSelectionShape::Circle:
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

            case EOdysseyVectorSelectionShape::Freehand : 
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

void
FOdysseyPainterEditorVectorSelectionToolHUD::Draw( BLContext* iBLContext
                                                 , FOdysseyVectorScene* iScene )
{
    uint64 hudFlags = GetViewingMode();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint32 selectedObjectCount = iScene->GetSelectedObjectList().size();

    // Draw object details only in vertex mode
    if( hudFlags & VIEW_MODE_VERTEX )
    {
        // static call
        FOdysseyVectorHUD::DrawObjects( iBLContext
                                      , iScene
                                      , fgColor
                                      , bgColor
                                      , hcColor
                                      , hudFlags | VIEW_PATH_VERTEX | VIEW_PATH_SEGMENT );
    }

    if( hudFlags & VIEW_MODE_OBJECT )
    {
        DrawSelectionBox( iBLContext, iScene, fgColor, bgColor, hcColor, hudFlags );
    }

    if( ( mShowSelectionIfEmpty == true ) || ( selectedObjectCount > 0 ) )
    {
        DrawSelectionSpace( iBLContext, iScene, hudFlags );

        if( mShowSelectionBox )
        {
            //DrawSelectionBox( iBLContext, iScene, iDrawingFlags );
        }
    }

    iBLContext->save();
    iBLContext->resetMatrix();

    DrawPickingArea( iBLContext, fgColor, bgColor, hcColor );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::ClearMask()
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();
}

::ULIS::FRectD
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateCircleMask( double iX, double iY, double iRadius )
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 1.0f );
    mBLSelectionContext.fillCircle( iX, iY, iRadius );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    return ::ULIS::FRectD::FromMinMax( iX - iRadius, iY - iRadius
                                     , iX + iRadius, iY + iRadius );
}

::ULIS::FRectD
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateRectangleMask( const ::ULIS::FRectD& iRect )
{
    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp( BL_COMP_OP_SRC_COPY );
    mBLSelectionContext.setFillAlpha( 0.0f );
    mBLSelectionContext.clearAll();
    mBLSelectionContext.setFillAlpha( 1.0f );
    mBLSelectionContext.fillRect( iRect.x, iRect.y, iRect.w, iRect.h );
    mBLSelectionContext.flush( BL_CONTEXT_FLUSH_SYNC );

    mBLSelectionContext.restore();

    return iRect;
}

::ULIS::FRectD
FOdysseyPainterEditorVectorSelectionToolHUD::GenerateFreehandMask( std::vector<::ULIS::FVec2D>& iPointArray )
{
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };
    BLPath path;

    mBLSelectionContext.save();

    mBLSelectionContext.setCompOp(BL_COMP_OP_SRC_COPY);
    /*blctx.setFillStyle( BLRgba32(0x00000000) );*/
    mBLSelectionContext.setFillAlpha(0.0f);
    mBLSelectionContext.clearAll();

    if( iPointArray.size() )
    {
        double x1 = iPointArray[0].x, y1 = iPointArray[0].y
             , x2 = iPointArray[0].x, y2 = iPointArray[0].y;

        path.moveTo( iPointArray[0].x, iPointArray[0].y );

        for( uint32 i = 1; i < iPointArray.size(); i++ )
        {
            path.lineTo( iPointArray[i].x, iPointArray[i].y );

            if( iPointArray[i].x < x1 )
            {
                x1 = iPointArray[i].x;
            }

            if( iPointArray[i].y < y1 )
            {
                y1 = iPointArray[i].y;
            }

            if( iPointArray[i].x > x2 )
            {
                x2 = iPointArray[i].x;
            }

            if( iPointArray[i].y > y2 )
            {
                y2 = iPointArray[i].y;
            }
        }

        rect = ::ULIS::FRectD::FromMinMax( x1, y1, x2, y2 );

        mBLSelectionContext.setFillAlpha(1.0f);
        mBLSelectionContext.fillPath( path );
    }

    mBLSelectionContext.flush(BL_CONTEXT_FLUSH_SYNC);
    mBLSelectionContext.restore();

    return rect;
}
