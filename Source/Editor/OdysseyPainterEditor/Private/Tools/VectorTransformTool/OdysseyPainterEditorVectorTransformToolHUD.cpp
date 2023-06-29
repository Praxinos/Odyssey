#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorTransformToolHUD::~FOdysseyPainterEditorVectorTransformToolHUD()
{
}

FOdysseyPainterEditorVectorTransformToolHUD::FOdysseyPainterEditorVectorTransformToolHUD( UOdysseyPainterEditorVectorTransformTool* iTransformTool )
    : FOdysseyVectorHUDSelection()
    , mFlags( 0 )
    , mShowSelectionBox( true )
{
    mTransformTool = iTransformTool;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::GetFlags()
{
    return mFlags;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::ShowSelectionBox( bool iShowSelectionBox )
{
    mShowSelectionBox = iShowSelectionBox;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::SetCursorPosition( double iWorldX, double iWorldY )
{
//    mX = iX;
//    mY = iY;

    Pick( iWorldX, iWorldY );

    return mFlags;
}

static void
GetWorldGizmo( FSelectionBox& iSelectionBox
             , ::ULIS::FVec2D& iGizmo
             , ::ULIS::FVec2D& oWorldPosition
             , ::ULIS::FVec2D& oWorldXAxis
             , ::ULIS::FVec2D& oWorldYAxis  )
{
    BLPoint worldGizmo = iSelectionBox.worldMatrix.mapPoint( iGizmo.x, iGizmo.y );
    BLPoint worldXAxis = iSelectionBox.worldMatrix.mapVector( 1.0f, 0.0f );
    BLPoint worldYAxis = iSelectionBox.worldMatrix.mapVector( 0.0f, 1.0f );
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

typedef struct _FSelectionBoxScaler
{
    BLPoint position;
    uint32 flag;
}
FSelectionBoxScaler;

static void
GetWorldScalers( FSelectionBox& iSelectionBox, FSelectionBoxScaler iScaler[4] )
{
    BLPoint handle[4] = { iSelectionBox.worldMatrix.mapPoint( iSelectionBox.rect.x                       , iSelectionBox.rect.y                        )
                        , iSelectionBox.worldMatrix.mapPoint( iSelectionBox.rect.x + iSelectionBox.rect.w, iSelectionBox.rect.y                        )
                        , iSelectionBox.worldMatrix.mapPoint( iSelectionBox.rect.x + iSelectionBox.rect.w, iSelectionBox.rect.y + iSelectionBox.rect.h )
                        , iSelectionBox.worldMatrix.mapPoint( iSelectionBox.rect.x                       , iSelectionBox.rect.y + iSelectionBox.rect.h ) };

    iScaler[0].position = handle[0];
    iScaler[0].flag     = FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT;
    iScaler[1].position = handle[1];
    iScaler[1].flag     = FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT;
    iScaler[2].position = handle[2];
    iScaler[2].flag     = FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT;
    iScaler[3].position = handle[3];
    iScaler[3].flag     = FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::DrawScalers( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    FSelectionBoxScaler scaler[4];

    GetWorldScalers( mSelectionBox, scaler);

    for( int i = 0; i < 4; i++ )
    {
        BLRgba32 scalerColor = ( mFlags & scaler[i].flag ) ? BLRgba32( 0xFF0000FF )
                                                           : BLRgba32( 0xFFD0E040 );// teal (ABGR)

        blctx->setFillStyle( scalerColor );
        blctx->fillCircle( scaler[i].position.x, scaler[i].position.y, SCALER_RADIUS );
        blctx->setStrokeWidth( 1.0f );
        blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
        blctx->strokeCircle( scaler[i].position.x, scaler[i].position.y, SCALER_RADIUS );
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
    ::ULIS::FVec2D worldGizmo;
    ::ULIS::FVec2D worldXAxis;
    ::ULIS::FVec2D worldYAxis;

    GetWorldGizmo( mSelectionBox, mGizmo, worldGizmo, worldXAxis, worldYAxis );

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

void
FOdysseyPainterEditorVectorTransformToolHUD::Pick( double iWorldX, double iWorldY )
{
    uint32 newFlags = 0;

    mFlags &= (~PICK_CHANGED);

    if( mSelectionBox.rect.Area() )
    {
        newFlags = PickGizmo( iWorldX, iWorldY );

        if( newFlags == 0 )
        {
            newFlags = PickScalers( iWorldX, iWorldY );

            if( newFlags == 0 )
            {
                newFlags = PickSelectionBox( iWorldX, iWorldY );
            }
        }
    }

    mFlags = newFlags | ( ( mFlags != newFlags ) ? PICK_CHANGED : 0 );
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::PickScalers( double iWorldX, double iWorldY )
{
    uint32 newFlags = 0;
    FSelectionBoxScaler scaler[4];

    GetWorldScalers( mSelectionBox, scaler );

    for( int i = 0; i < 4; i++ )
    {
        double distToScaler = ::ULIS::FVec2D( iWorldX - scaler[i].position.x
                                            , iWorldY - scaler[i].position.y ).Distance();

        if( distToScaler < mTransformTool->PickingRadius )
        {
            newFlags = scaler[i].flag;
        }
    }

    return newFlags;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::PickSelectionBox( double iWorldX, double iWorldY )
{
    BLPoint localPt = mSelectionBox.inverseWorldMatrix.mapPoint( iWorldX, iWorldY );
    uint32 newFlags = 0;

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

    return newFlags;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::PickGizmo( double iWorldX, double iWorldY )
{
    ::ULIS::FVec2D worldGizmo;
    ::ULIS::FVec2D worldXAxis;
    ::ULIS::FVec2D worldYAxis;
    double distToXAxis = DBL_MAX;
    double distToYAxis = DBL_MAX;
    ::ULIS::FVec2D toPivot;
    uint32 newFlags = 0;

    GetWorldGizmo( mSelectionBox, mGizmo, worldGizmo, worldXAxis, worldYAxis );

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

        if( mShowSelectionBox )
        {
            DrawSelectionBox( iScene, iFlags );
            DrawScalers( iScene, iFlags );
        }

        DrawGizmo( iScene, iFlags );
    }

    blctx->restore();
}
