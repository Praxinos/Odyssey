#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorPickToolHUD::~FOdysseyPainterEditorVectorPickToolHUD()
{
    delete mSelectionMask;
}

FOdysseyPainterEditorVectorPickToolHUD::FOdysseyPainterEditorVectorPickToolHUD( UOdysseyPainterEditorVectorPickTool* iPickTool )
    : mSelectionMask ( nullptr )
    , mShowSelectionBox( true )
{
    mPickTool = iPickTool;
}

void
FOdysseyPainterEditorVectorPickToolHUD::Load( FOdysseyVectorScene* iScene )
{
    uint32 width, height;

    iScene->GetEngine()->GetColorImageSize( &width, &height );

    if( mSelectionMask )
    {
        delete mSelectionMask;
    }

    mSelectionMask = new BLImage( width, height, BL_FORMAT_A8 );
}

void
FOdysseyPainterEditorVectorPickToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    UpdateSelectionBox( iScene, mPickTool->World );
}

FSelectionBox&
FOdysseyPainterEditorVectorPickToolHUD::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyPainterEditorVectorPickToolHUD::ShowSelectionBox( bool iShowSelectionBox )
{
    mShowSelectionBox = iShowSelectionBox;
}

void
FOdysseyPainterEditorVectorPickToolHUD::GetSelectedVertices( FOdysseyVectorScene* iScene
                                                           , std::vector<FOdysseyVectorPoint*>& oPointArray )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mPickTool->GetFocusedObjectList( iScene );
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
FOdysseyPainterEditorVectorPickToolHUD::DrawSelectionSpace( FOdysseyVectorScene* iScene
                                                          , uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    BLPoint topLeft = { 0, 0 };

    blctx->save();
    blctx->resetMatrix();

    if( iScene->GetEngine()->GetSelectionSpace() )
    {
        FOdysseyVectorGroup* selectionSpace = iScene->GetEngine()->GetSelectionSpace();
        ::ULIS::FRectD selectionSpaceBBox = selectionSpace->GetBBox( false );
        BLRgba32 strokeColor = { 0x80, 0x80, 0x80, 0xFF };
        BLMatrix2D& worldMatrix = selectionSpace->GetWorldMatrix();

        iScene->GetEngine()->UseImage( mSelectionMask );

        blctx->setCompOp( BL_COMP_OP_SRC_COPY );

        blctx->clearAll();
        blctx->setFillStyle( BLRgba32( 0x80FFFFFF ) );
        blctx->fillRect( 0, 0, mSelectionMask->width(), mSelectionMask->height() );

        blctx->setMatrix( worldMatrix );
        blctx->setFillStyle( BLRgba32( 0x00000000/*0x800000FF*/ ) );
        blctx->fillRect( selectionSpaceBBox.x, selectionSpaceBBox.y, selectionSpaceBBox.w, selectionSpaceBBox.h );

        iScene->GetEngine()->UseColorImage();

        blctx->blitImage( topLeft, *mSelectionMask );
    }

    blctx->restore();
}

void
FOdysseyPainterEditorVectorPickToolHUD::UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene
                                                                    , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mPickTool->GetFocusedObjectList( iScene );
    std::list<FOdysseyVectorObject*>::iterator it;
    bool inited = false;

    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix.reset();

    for( it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = *it;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(selectedObject);
            ::ULIS::FRectD selectedPathBBox;

            if( selectedPath->GetBBoxFromSelectedVertices( selectedPathBBox, true ) )
            {
                mSelectionBox.rect = inited ? mSelectionBox.rect | selectedPathBBox
                                            : selectedPathBBox;

                inited = true;
            }
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* selectedPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);
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
FOdysseyPainterEditorVectorPickToolHUD::UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene
                                                                    , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mPickTool->GetFocusedObjectList( iScene );

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
FOdysseyPainterEditorVectorPickToolHUD::UpdateSelectionBox( FOdysseyVectorScene* iScene
                                                          , bool iForceWorld )
{
    switch( mPickTool->GetEditor()->GetVectorEditionMode() )
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
FOdysseyPainterEditorVectorPickToolHUD::DrawSelectionBox( FOdysseyVectorScene* iScene
                                                        , uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

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

        blctx->setStrokeStyle( bgColor );
        blctx->setStrokeWidth( 2.0f );
        blctx->strokePath( path );

        blctx->setStrokeStyle( fgColor );
        blctx->setStrokeWidth( 1.0f );
        blctx->strokePath( path );
    }

    blctx->restore();
}

void
FOdysseyPainterEditorVectorPickToolHUD::DrawObjectSelection( FOdysseyVectorScene* iScene
                                                           , uint64 iFlags )
{
}

void
FOdysseyPainterEditorVectorPickToolHUD::DrawVertexSelection( FOdysseyVectorScene* iScene
                                                           , uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mPickTool->GetFocusedObjectList( iScene );
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

            FOdysseyVectorHUD::DrawPath( path, fgColor, bgColor, hcColor, true, VIEW_VERTEX | VIEW_SEGMENT );
        }

        if( selectedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            FOdysseyVectorHUD::DrawPaintGroup( paintGroup, fgColor, bgColor, hcColor, true, VIEW_VERTEX | VIEW_SEGMENT );
        }
    }
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

    switch( mPickTool->GetEditor()->GetVectorEditionMode() )
    {
        case eVectorEditionMode::Object :
            DrawObjectSelection( iScene, iFlags );
        break;

        case eVectorEditionMode::Vertex :
            DrawVertexSelection( iScene, iFlags );
        break;

        default:
        break;
    }

    DrawSelectionSpace( iScene, iFlags );

    if( mShowSelectionBox )
    {
        DrawSelectionBox( iScene, iFlags );
    }

    blctx->save();
    blctx->resetMatrix();

    DrawPickingArea( blctx, fgColor, bgColor, hcColor );

    blctx->restore();
}
