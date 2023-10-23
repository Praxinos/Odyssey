#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorSelectionToolHUD::~FOdysseyPainterEditorVectorSelectionToolHUD()
{

}

FOdysseyPainterEditorVectorSelectionToolHUD::FOdysseyPainterEditorVectorSelectionToolHUD( UOdysseyPainterEditorVectorSelectionTool* iSelectionTool )
    : mShowSelectionBox( true )
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
    UpdateSelectionBox( iScene, false );
}

BLImage*
FOdysseyPainterEditorVectorSelectionToolHUD::GetMask()
{
    return &mBLSelectionMask;
}

FSelectionBox&
FOdysseyPainterEditorVectorSelectionToolHUD::GetSelectionBox()
{
    return mSelectionBox;
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
    std::list<FOdysseyVectorObject*>& selectedObjectList = mSelectionTool->GetFocusedObjectList( iScene );
    std::list<FOdysseyVectorObject*>::iterator it;

    // avoir to many reallocation by reserving a decent amount of memory
    oPointArray.reserve( 200 );

    for( it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = *it;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);

            selectedPath->GetSelectedPoints( oPointArray
                                           , ePointSelectionFlags::Vertex
                                           | ePointSelectionFlags::Strict
                                           | ePointSelectionFlags::SegmentHandle );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            selectedPaintGroup->GetSelectedPoints( oPointArray
                                                 , ePointSelectionFlags::Vertex
                                                 | ePointSelectionFlags::SegmentHandle
                                                 | ePointSelectionFlags::Bucket );
        }
    }
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
FOdysseyPainterEditorVectorSelectionToolHUD::UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene
                                                                         , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mSelectionTool->GetFocusedObjectList( iScene );
    bool inited = false;

    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix.reset();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(focusedObject);
            ::ULIS::FRectD selectedPathBBox;

            if( selectedPath->GetBBoxFromSelectedVertices( selectedPathBBox, true ) )
            {
                mSelectionBox.rect = inited ? mSelectionBox.rect | selectedPathBBox
                                            : selectedPathBBox;

                inited = true;
            }
        }

        if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);
            ::ULIS::FRectD selectedPaintGroupBBox;

            if( selectedPaintGroup->GetBBoxFromSelectedVertices( selectedPaintGroupBBox, true ) )
            {
                mSelectionBox.rect = inited ? mSelectionBox.rect | selectedPaintGroupBBox
                                            : selectedPaintGroupBBox;

                inited = true;
            }
        }
    }

    BLMatrix2D::invert( mSelectionBox.inverseWorldMatrix, mSelectionBox.worldMatrix );

    /*if( inited )
    {
        ::ULIS::FVec2D origin = ::ULIS::FVec2D( mSelectionBox.rect.x + ( mSelectionBox.rect.w * 0.5f )
                                              , mSelectionBox.rect.y + ( mSelectionBox.rect.h * 0.5f ) );

        mSelectionBox.worldMatrix.translate( origin.x, origin.y );
    }*/
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene
                                                                         , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mSelectionTool->GetFocusedObjectList( iScene );

    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );

    if( selectedObjectList.size() )
    {
        if( ( selectedObjectList.size() == 1 ) && ( iForceWorld == false ) )
        {
            FOdysseyVectorObject* selectedObject = selectedObjectList.front();

            mSelectionBox.rect = selectedObject->GetBBox( false );

            mSelectionBox.worldMatrix = selectedObject->GetWorldMatrix();
            mSelectionBox.inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
        }
        else
        {
            BLPoint p0, p1, p2, p3;
            ::ULIS::FRectD rect = FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );
            ::ULIS::FVec2D origin = ::ULIS::FVec2D( rect.x + (rect.w * 0.5f)
                                                  , rect.y + (rect.h * 0.5f) );
            mSelectionBox.worldMatrix.reset();
            mSelectionBox.worldMatrix.translate( origin.x, origin.y );

            BLMatrix2D::invert( mSelectionBox.inverseWorldMatrix, mSelectionBox.worldMatrix );

            p0 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y          );
            p1 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y          );
            p2 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y + rect.h );
            p3 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y + rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                           , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
        }
    }
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::UpdateSelectionBox( FOdysseyVectorScene* iScene
                                                               , bool iForceWorld )
{
    switch( mSelectionTool->GetEditor()->GetVectorEditionMode() )
    {
        case eVectorEditionMode::Object :
            UpdateSelectionBoxObjectMode( iScene, iForceWorld );
        break;

        case eVectorEditionMode::Vertex:
            UpdateSelectionBoxVertexMode( iScene, iForceWorld );
        break;

        default :
        break;
    }
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawSelectionBox( BLContext* iBLContext
                                                             , FOdysseyVectorScene* iScene
                                                             , uint64 iFlags )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    BLRgba32 white = BLRgba32( 255, 255, 255, 255 );
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    iBLContext->save();
    iBLContext->resetMatrix();

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };
        BLPath path;

        path.moveTo( point[0] );
        path.lineTo( point[1] );
        path.lineTo( point[2] );
        path.lineTo( point[3] );
        path.close();

        iBLContext->setStrokeStyle( bgColor );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        // draw box as white if nothing is selected, colored if something is selected
        iBLContext->setStrokeStyle( ( iScene->GetSelectedObjectList().size() == 0 ) ? white : fgColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawObjectSelection( BLContext* iBLContext
                                                                , FOdysseyVectorScene* iScene
                                                                , uint64 iFlags )
{
}

void
FOdysseyPainterEditorVectorSelectionToolHUD::DrawVertexSelection( BLContext* iBLContext
                                                                , FOdysseyVectorScene* iScene
                                                                , uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mSelectionTool->GetFocusedObjectList( iScene );
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

            FOdysseyVectorHUD::DrawPath( iBLContext, path, fgColor, bgColor, hcColor, true, VIEW_VERTEX | VIEW_SEGMENT );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            FOdysseyVectorHUD::DrawPaintGroup( iBLContext, paintGroup, fgColor, bgColor, hcColor, true, VIEW_VERTEX | VIEW_SEGMENT );
        }
    }
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
                                                 , FOdysseyVectorScene* iScene
                                                 , uint64 iFlags )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint32 selectedObjectCount = iScene->GetSelectedObjectList().size();

    if( ( mShowSelectionIfEmpty == true ) || ( selectedObjectCount > 0 ) )
    {
        switch( mSelectionTool->GetEditor()->GetVectorEditionMode() )
        {
            case eVectorEditionMode::Object :
                DrawObjectSelection( iBLContext, iScene, iFlags );
            break;

            case eVectorEditionMode::Vertex :
                DrawVertexSelection( iBLContext, iScene, iFlags );
            break;

            default:
            break;
        }

        DrawSelectionSpace( iBLContext, iScene, iFlags );

        if( mShowSelectionBox )
        {
            DrawSelectionBox( iBLContext, iScene, iFlags );
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
