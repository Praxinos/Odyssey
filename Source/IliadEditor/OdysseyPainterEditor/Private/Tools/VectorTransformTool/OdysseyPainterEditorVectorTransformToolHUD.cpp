// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVector.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorCell.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorVectorTransformToolHUD::~FOdysseyPainterEditorVectorTransformToolHUD()
{
}

FOdysseyPainterEditorVectorTransformToolHUD::FOdysseyPainterEditorVectorTransformToolHUD( UOdysseyPainterEditorVectorTransformTool* iTransformTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iTransformTool )
    , mFlags( 0 )
    , mCenterGizmo( true )
    , mShowSelectionBox( true )
    , mShowSelectionIfEmpty ( false )
{
    mTransformTool = iTransformTool;

    mShowSelectionIfEmpty = true;
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::GetFlags()
{
    return mFlags;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::SetCenterGizmo( bool iCenterGizmo )
{
    mCenterGizmo = iCenterGizmo;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::ShowSelectionBox( bool iShowSelectionBox )
{
    mShowSelectionBox = iShowSelectionBox;
}

void
FOdysseyPainterEditorVectorTransformToolHUD::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    FOdysseyPainterEditorVectorBaseToolHUD::SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    Pick( iPointInTexture.x, iPointInTexture.y );
}

static void
GetWorldGizmo( FSelectionBox& iSelectionBox
             , ::ULIS::FVec2D& iGizmo
             , ::ULIS::FVec2D& oWorldPosition
             , ::ULIS::FVec2D& oWorldXAxisStart
             , ::ULIS::FVec2D& oWorldYAxisStart
             , ::ULIS::FVec2D& oWorldXAxisLength
             , ::ULIS::FVec2D& oWorldYAxisLength  )
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
        oWorldXAxisStart  = ::ULIS::FVec2D( worldGizmo.x, worldGizmo.y ) + ( XAxis * FOdysseyPainterEditorVectorTransformToolHUD::GIZMO_RADIUS );
        oWorldXAxisLength = XAxis * FOdysseyPainterEditorVectorTransformToolHUD::AXIS_LENGTH;
    }

    if( YAxis.DistanceSquared() )
    {
        YAxis.Normalize();
        oWorldYAxisStart  = ::ULIS::FVec2D( worldGizmo.x, worldGizmo.y ) + ( YAxis * FOdysseyPainterEditorVectorTransformToolHUD::GIZMO_RADIUS );
        oWorldYAxisLength = YAxis * FOdysseyPainterEditorVectorTransformToolHUD::AXIS_LENGTH;
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
FOdysseyPainterEditorVectorTransformToolHUD::DrawScalers( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    FSelectionBoxScaler texScaler[4];

    GetWorldScalers( mSelectionBox, texScaler);

    for( int i = 0; i < 4; i++ )
    {
        FLinearColor scalerColor = ( mFlags & texScaler[i].flag ) ? hcColor : fgColor;
        FVector2D hudCoords = iParams.mTextureToHUD.Execute( FVector2D( texScaler[i].position.x
                                                                      , texScaler[i].position.y ) );

        DrawPrimitiveHandle( iParams, hudCoords, SCALER_RADIUS, scalerColor, bgColor );
    }
}

void
FOdysseyPainterEditorVectorTransformToolHUD::DrawGizmo( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    FLinearColor gizmoColor = ( mFlags & PICK_ZAXIS ) ? hcColor : fgColor;
    FLinearColor xAxisColor = ( mFlags & PICK_XAXIS ) ? hcColor : fgColor;
    FLinearColor yAxisColor = ( mFlags & PICK_YAXIS ) ? hcColor : fgColor;
    ::ULIS::FVec2D worldXAxisStart;
    ::ULIS::FVec2D worldYAxisStart;
    ::ULIS::FVec2D worldXAxisVector;
    ::ULIS::FVec2D worldYAxisVector;
    ::ULIS::FVec2D worldGizmo;
    FVector2D hudXAxisStart;
    FVector2D hudYAxisStart;
    FVector2D hudXAxisVector;
    FVector2D hudYAxisVector;

    GetWorldGizmo( mSelectionBox
                 , mGizmo
                 , worldGizmo
                 , worldXAxisStart
                 , worldYAxisStart
                 , worldXAxisVector
                 , worldYAxisVector );

    hudXAxisStart = WorldPointToHUD( iParams, FVector2D( worldXAxisStart.x, worldXAxisStart.y ) );
    hudYAxisStart = WorldPointToHUD( iParams, FVector2D( worldYAxisStart.x, worldYAxisStart.y ) );
    hudXAxisVector = WorldVectorToHUD( iParams
                                     , hudXAxisStart
                                     , FVector2D( worldXAxisVector.x
                                                , worldXAxisVector.y ) );
    hudYAxisVector = WorldVectorToHUD( iParams
                                     , hudYAxisStart
                                     , FVector2D( worldYAxisVector.x
                                                , worldYAxisVector.y ) );

    // Central circle
/*
    iBLContext->setFillStyle( gizmoColor );
    iBLContext->fillCircle( worldGizmo.x, worldGizmo.y, GIZMO_RADIUS );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->setStrokeStyle( bgColor );
    iBLContext->strokeCircle( worldGizmo.x, worldGizmo.y, GIZMO_RADIUS );
*/
    // Axises
    DrawPrimitiveLine( iParams, hudXAxisStart, hudXAxisStart + hudXAxisVector, bgColor, 2.0f );
    DrawPrimitiveLine( iParams, hudYAxisStart, hudYAxisStart + hudYAxisVector, bgColor, 2.0f );

    DrawPrimitiveLine( iParams, hudXAxisStart, hudXAxisStart + hudXAxisVector, xAxisColor, 1.0f );
    DrawPrimitiveLine( iParams, hudYAxisStart, hudYAxisStart + hudYAxisVector, yAxisColor, 1.0f );
}

uint32
FOdysseyPainterEditorVectorTransformToolHUD::Pick( double iWorldX, double iWorldY )
{
    uint32 newFlags = 0;

    //mFlags &= (~PICK_CHANGED);
    mFlags = 0;

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

    return mFlags;
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
    ::ULIS::FVec2D worldXAxisStart;
    ::ULIS::FVec2D worldYAxisStart;
    ::ULIS::FVec2D worldXAxisLength;
    ::ULIS::FVec2D worldYAxisLength;
    double distToXAxis = DBL_MAX;
    double distToYAxis = DBL_MAX;
    ::ULIS::FVec2D toPivot;
    uint32 newFlags = 0;

    GetWorldGizmo( mSelectionBox
                 , mGizmo
                 , worldGizmo
                 , worldXAxisStart
                 , worldYAxisStart
                 , worldXAxisLength
                 , worldYAxisLength );

    toPivot = ::ULIS::FVec2D( worldGizmo.x - iWorldX, worldGizmo.y - iWorldY );

    if( toPivot.Distance() < mTransformTool->PickingRadius )
    {
        newFlags = PICK_ZAXIS;
    }
    else
    {
        FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D( iWorldX, iWorldY )
                                         , ::ULIS::FVec2D( worldXAxisStart.x
                                                         , worldXAxisStart.y )
                                         , ::ULIS::FVec2D( worldXAxisStart.x + worldXAxisLength.x
                                                         , worldXAxisStart.y + worldXAxisLength.y )
                                         , distToXAxis );

        if( distToXAxis < mTransformTool->PickingRadius )
        {
            newFlags = PICK_XAXIS;
        }

        FOdysseyVector::DistanceToSegment( ::ULIS::FVec2D( iWorldX, iWorldY )
                                         , ::ULIS::FVec2D( worldYAxisStart.x
                                                         , worldYAxisStart.y )
                                         , ::ULIS::FVec2D( worldYAxisStart.x + worldYAxisLength.x
                                                         , worldYAxisStart.y + worldYAxisLength.y )
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
FOdysseyPainterEditorVectorTransformToolHUD::CenterGizmo()
{
    SetGizmo( mSelectionBox.rect.x + ( mSelectionBox.rect.w * 0.5f )
            , mSelectionBox.rect.y + ( mSelectionBox.rect.h * 0.5f ) );
}

void
FOdysseyPainterEditorVectorTransformToolHUD::Reset( )
{
    uint64 hudFlags = mTransformTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateSelectionInbetweenMode( true );
    }

    UpdateSelectionBox( mTransformTool->World, mTransformTool->GetEditor()->GetVectorHUDFlags() );

    if( mCenterGizmo )
    {
        CenterGizmo();
    }
}

void
FOdysseyPainterEditorVectorTransformToolHUD::DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams )
{
    FLinearColor fgColor = FLinearColor( FOdysseyVectorHUD::GetForegroundColor() );
    FLinearColor bgColor = FLinearColor( FOdysseyVectorHUD::GetBackgroundColor() );
    FLinearColor hcColor = FLinearColor( FOdysseyVectorHUD::GetHighlightColor() );
    uint32 selectedObjectCount = mScene->GetCell()->GetSelectedObjectList().size();
    uint64 hudFlags = mTransformTool->GetEditor()->GetVectorHUDFlags();

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        FOdysseyPainterEditorVectorBaseToolHUD::DrawHUD( iParams );
    }

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        for( FInbetweenerBreakdown* breakdown : mSelectedBreakdownList )
        {
            DrawInbetweens( iParams
                          , breakdown
                          , mTransformTool->ShowInbetweens
                          , 0
                          , 0 );
        }
    }

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D worldMatrix = mSelectionBox.worldMatrix;

        if( mShowSelectionBox )
        {
            DrawSelectionBox( iParams, fgColor, bgColor, hcColor, hudFlags );

            DrawScalers( iParams );
        }

        DrawGizmo( iParams );
    }

    // invisible plane will get mouse events
    DrawDummyPlane( iParams );
}

void
FOdysseyPainterEditorVectorTransformToolHUD::Draw( BLContext* iBLContext )
{
}

#undef LOCTEXT_NAMESPACE
