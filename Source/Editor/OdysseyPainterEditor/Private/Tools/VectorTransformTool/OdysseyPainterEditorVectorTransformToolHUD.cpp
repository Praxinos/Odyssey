#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorTransformToolHUD::~FOdysseyPainterEditorVectorTransformToolHUD()
{
}

FOdysseyPainterEditorVectorTransformToolHUD::FOdysseyPainterEditorVectorTransformToolHUD( UOdysseyPainterEditorVectorTransformTool* iTransformTool )
    : FOdysseyVectorHUDSelection()
    , mFlags( 0 )
{
    mTransformTool = iTransformTool;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::GetFlags()
{
    return mFlags;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::SetCursorPosition( double iWorldX, double iWorldY )
{
//    mX = iX;
//    mY = iY;

    Pick( iWorldX, iWorldY );

    return mFlags;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::GetWorldGizmo( ::ULIS::FVec2D& oWorldPosition
                                                          , ::ULIS::FVec2D& oWorldXAxis
                                                          , ::ULIS::FVec2D& oWorldYAxis  )
{
    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint worldGizmo = worldMatrix.mapPoint( mGizmo.x, mGizmo.y );
        BLPoint worldXAxis = worldMatrix.mapVector( 1.0f, 0.0f );
        BLPoint worldYAxis = worldMatrix.mapVector( 0.0f, 1.0f );
        ::ULIS::FVec2D XAxis = ::ULIS::FVec2D( worldXAxis.x, worldXAxis.y );
        ::ULIS::FVec2D YAxis = ::ULIS::FVec2D( worldYAxis.x, worldYAxis.y );

        oWorldPosition = ::ULIS::FVec2D( worldGizmo.x, worldGizmo.y );

        if( XAxis.DistanceSquared() )
        {
            XAxis.Normalize();
            oWorldXAxis = XAxis * 80.0f;
        }

        if( YAxis.DistanceSquared() )
        {
            YAxis.Normalize();
            oWorldYAxis = YAxis * 80.0f;
        }
    }
}

void
FOdysseyPainterEditorVectorTransformToolHUD::DrawGizmo( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    BLRgba32 gizmoColor = ( mFlags & PICK_ZAXIS ) ? BLRgba32( 0xFF0000FF )
                                                       : BLRgba32( 0xFFD0E040 );// teal (ABGR)
    BLRgba32 xAxisColor = ( mFlags & PICK_XAXIS ) ? BLRgba32( 0xFF0000FF )
                                                       : BLRgba32( 0xFFD0E040 );// teal (ABGR)
    BLRgba32 yAxisColor = ( mFlags & PICK_YAXIS ) ? BLRgba32( 0xFF0000FF )
                                                       : BLRgba32( 0xFFD0E040 );// teal (ABGR)

    blctx->save();
    blctx->resetMatrix();

    if( mSelectionBox.rect.Area() )
    {
        ::ULIS::FVec2D worldGizmo;
        ::ULIS::FVec2D worldXAxis;
        ::ULIS::FVec2D worldYAxis;

        GetWorldGizmo( worldGizmo, worldXAxis, worldYAxis );

        // Central circle

        blctx->setFillStyle( gizmoColor );
        blctx->fillCircle( worldGizmo.x, worldGizmo.y, GIZMO_RADIUS );
        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
        blctx->strokeCircle( worldGizmo.x, worldGizmo.y, GIZMO_RADIUS );

        // Axises

        blctx->setStrokeWidth( 2.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
        blctx->strokeLine( worldGizmo.x + GIZMO_RADIUS
                         , worldGizmo.y
                         , worldGizmo.x + worldXAxis.x
                         , worldGizmo.y + worldXAxis.y );
        blctx->strokeLine( worldGizmo.x
                         , worldGizmo.y + GIZMO_RADIUS
                         , worldGizmo.x + worldYAxis.x
                         , worldGizmo.y + worldYAxis.y );

        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( xAxisColor );
        blctx->strokeLine( worldGizmo.x + GIZMO_RADIUS
                         , worldGizmo.y
                         , worldGizmo.x + worldXAxis.x
                         , worldGizmo.y + worldXAxis.y );
        blctx->setStrokeStyle( yAxisColor );
        blctx->strokeLine( worldGizmo.x
                         , worldGizmo.y + GIZMO_RADIUS
                         , worldGizmo.x + worldYAxis.x
                         , worldGizmo.y + worldYAxis.y );
    }

    blctx->restore();
}

void
FOdysseyPainterEditorVectorTransformToolHUD::Pick( double iWorldX, double iWorldY )
{
    uint32 newFlags = 0;

    mFlags &= (~PICK_CHANGED);

    newFlags = PickGizmo( iWorldX, iWorldY );

    if( newFlags == 0 )
    {
        newFlags = PickSelectionBox( iWorldX, iWorldY );
    }

    mFlags = newFlags | ( ( mFlags != newFlags ) ? PICK_CHANGED : 0 );
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::PickSelectionBox( double iWorldX, double iWorldY )
{
    uint32 newFlags = 0;

    if( mSelectionBox.rect.Area() )
    {
        BLPoint localPt = mSelectionBox.inverseWorldMatrix.mapPoint( iWorldX, iWorldY );

        // inside = translate
        if( mSelectionBox.rect.HitTest( ::ULIS::FVec2D( localPt.x, localPt.y ) ) )
        {
            newFlags = PICK_TRANSLATE;
        }
        // outside = rotate
        else
        {
            newFlags = PICK_ROTATE;
        }
    }

    return newFlags;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::PickGizmo( double iWorldX, double iWorldY )
{
    uint32 newFlags = 0;

    if( mSelectionBox.rect.Area() )
    {
        ::ULIS::FVec2D worldGizmo;
        ::ULIS::FVec2D worldXAxis;
        ::ULIS::FVec2D worldYAxis;
        double distToXAxis = DBL_MAX;
        double distToYAxis = DBL_MAX;
        ::ULIS::FVec2D toPivot;

        GetWorldGizmo( worldGizmo, worldXAxis, worldYAxis );

        toPivot = ::ULIS::FVec2D( worldGizmo.x - iWorldX, worldGizmo.y - iWorldY );

        if( toPivot.Distance() < GIZMO_RADIUS )
        {
            newFlags = PICK_ZAXIS;
        }

        FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D( iWorldX, iWorldY )
                                         , ::ULIS::FVec2D( worldGizmo.x + GIZMO_RADIUS
                                                         , worldGizmo.y )
                                         , ::ULIS::FVec2D( worldGizmo.x + worldXAxis.x
                                                         , worldGizmo.y + worldXAxis.y )
                                         , distToXAxis );

        if( distToXAxis < mTransformTool->PickingRadius )
        {
            newFlags = PICK_XAXIS;
        }

        FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D( iWorldX, iWorldY )
                                         , ::ULIS::FVec2D( worldGizmo.x
                                                         , worldGizmo.y + GIZMO_RADIUS )
                                         , ::ULIS::FVec2D( worldGizmo.x + worldYAxis.x
                                                         , worldGizmo.y + worldYAxis.y )
                                         , distToYAxis );

        if( distToYAxis < mTransformTool->PickingRadius )
        {
            newFlags = PICK_YAXIS;
        }
    }

    return newFlags;
}

::ULIS::FVec2D&
FOdysseyPainterEditorVectorTransformToolHUD::GetGizmo()
{
    return mGizmo;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::SetGizmo( double iLocalX, double iLocalY )
{
    mGizmo.x = iLocalX;
    mGizmo.y = iLocalY;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    FOdysseyVectorHUDSelection::Reset( iScene ); // Updates the selection box

    SetGizmo( mSelectionBox.rect.x + ( mSelectionBox.rect.w * 0.5f )
            , mSelectionBox.rect.y + ( mSelectionBox.rect.h * 0.5f ) );
}

void
FOdysseyPainterEditorVectorTransformToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    blctx->save();
    blctx->resetMatrix();

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        DrawSelectionBox( iScene, iFlags );

        DrawGizmo( iScene, iFlags );
    }

    blctx->restore();
}
