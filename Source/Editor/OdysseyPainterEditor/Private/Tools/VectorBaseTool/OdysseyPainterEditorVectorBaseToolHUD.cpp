#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorBaseToolHUD::~FOdysseyPainterEditorVectorBaseToolHUD()
{
}

FOdysseyPainterEditorVectorBaseToolHUD::FOdysseyPainterEditorVectorBaseToolHUD( UOdysseyPainterEditorVectorBaseTool* iBaseTool )
{
    mBaseTool = iBaseTool;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Load( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Unload( FOdysseyVectorScene* iScene )
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Reset( FOdysseyVectorScene* iScene )
{
    UpdateSelectionBox( iScene, false );
}

FSelectionBox&
FOdysseyPainterEditorVectorBaseToolHUD::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxVertexModeRecursive( FOdysseyVectorObject* iObject
                                                                             , bool iForceWorld )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* selectedPath = static_cast<FOdysseyVectorPath*>(iObject);
        ::ULIS::FRectD selectedPathBBox;

        if( selectedPath->GetBBoxFromSelectedVertices( selectedPathBBox, true ) )
        {
            mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | selectedPathBBox
                                                      : selectedPathBBox;

            mSelectionBox.inited = true;
        }
    }

    for( FOdysseyVectorObject* childObject : iObject->GetChildrenList() )
    {
        UpdateSelectionBoxVertexModeRecursive( childObject, iForceWorld );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene
                                                                    , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mBaseTool->GetFocusedObjectList( iScene );

    mSelectionBox.inited = false;
    //mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix.reset();

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        UpdateSelectionBoxVertexModeRecursive( focusedObject, iForceWorld );
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
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene
                                                                    , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mBaseTool->GetSelectedObjectList( iScene );

    mSelectionBox.inited = false;
    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );

    if( ( selectedObjectList.size() == 1 ) && ( iForceWorld == false ) )
    {
        FOdysseyVectorObject* selectedObject = selectedObjectList.front();

        mSelectionBox.inited = true;
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

        mSelectionBox.inited = true;
        mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                       , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                       , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                       , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBox( FOdysseyVectorScene* iScene
                                                          , bool iForceWorld )
{
    switch( mBaseTool->GetEditor()->GetVectorEditionMode() )
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
FOdysseyPainterEditorVectorBaseToolHUD::DrawSelectionBox( BLContext* iBLContext
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

    if( mSelectionBox.inited )
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
FOdysseyPainterEditorVectorBaseToolHUD::DrawObjectModeRecursive( BLContext* iBLContext
                                                               , FOdysseyVectorObject* iObject
                                                               , BLRgba32& iForegroundColor
                                                               , BLRgba32& iBackgroundColor
                                                               , BLRgba32& iHighlightColor
                                                               , uint64 iDrawingFlags )
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawObjectMode( BLContext* iBLContext
                                                      , FOdysseyVectorScene* iScene
                                                      , uint64 iDrawingFlags )
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawVertexModeRecursive( BLContext* iBLContext
                                                               , FOdysseyVectorObject* iObject
                                                               , BLRgba32& iForegroundColor
                                                               , BLRgba32& iBackgroundColor
                                                               , BLRgba32& iHighlightColor
                                                               , uint64 iDrawingFlags )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        FOdysseyVectorHUD::DrawPath( iBLContext
                                   , path
                                   , iForegroundColor
                                   , iBackgroundColor
                                   , iHighlightColor
                                   , true
                                   , VIEW_VERTEX | VIEW_SEGMENT );
    }

    for( FOdysseyVectorObject* child : iObject->GetChildrenList() )
    {
        DrawVertexModeRecursive( iBLContext
                               , child
                               , iForegroundColor
                               , iBackgroundColor
                               , iHighlightColor
                               , iDrawingFlags );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawVertexMode( BLContext* iBLContext
                                                      , FOdysseyVectorScene* iScene
                                                      , uint64 iDrawingFlags )
{
    std::list<FOdysseyVectorObject*>& focusedObjectList = mBaseTool->GetFocusedObjectList( iScene );
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    for( FOdysseyVectorObject* focusedObject : focusedObjectList )
    {
        DrawVertexModeRecursive( iBLContext
                               , focusedObject
                               , fgColor
                               , bgColor
                               , hcColor
                               , iDrawingFlags );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Draw( BLContext* iBLContext
                                            , FOdysseyVectorScene* iScene
                                            , uint64 iDrawingFlags )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );

    iBLContext->save();
    iBLContext->resetMatrix();

    switch( mBaseTool->GetEditor()->GetVectorEditionMode() )
    {
        case eVectorEditionMode::Object :
            DrawObjectMode( iBLContext, iScene, iDrawingFlags );
        break;

        case eVectorEditionMode::Vertex :
            DrawVertexMode( iBLContext, iScene, iDrawingFlags );
        break;

        default:
        break;
    }

    DrawSelectionBox( iBLContext, iScene, iDrawingFlags );

    iBLContext->restore();
}
