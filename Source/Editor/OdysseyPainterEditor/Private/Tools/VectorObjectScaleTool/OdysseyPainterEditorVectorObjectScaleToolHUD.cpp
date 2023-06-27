#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleToolHUD.h"
#include "OdysseyVectorEngine.h"

FOdysseyPainterEditorVectorObjectScaleToolHUD::~FOdysseyPainterEditorVectorObjectScaleToolHUD()
{
}

FOdysseyPainterEditorVectorObjectScaleToolHUD::FOdysseyPainterEditorVectorObjectScaleToolHUD( UOdysseyPainterEditorVectorObjectScaleTool* iObjectScaleTool )
    : mObjectScaleTool( iObjectScaleTool )
    , mHandleFlags( 0 )
{

}

void
FOdysseyPainterEditorVectorObjectScaleToolHUD::PickHandle( double iWorldX, double iWorldY )
{
    uint32 newHandleFlags = 0;

    mHandleFlags &= (~PICK_CHANGED);

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };

        // pick the handles
        for ( int i = 0; i < 4; i++ )
        {
            ::ULIS::FVec2D vec = { iWorldX - point[i].x, iWorldY - point[i].y };
 
            if ( vec.Distance() <= mObjectScaleTool->PickingRadius )
            {
                newHandleFlags = ( 1 << i );

                break;
            }
        }
    }

    mHandleFlags = newHandleFlags | ( ( mHandleFlags != newHandleFlags ) ? PICK_CHANGED : 0 );
}

void
FOdysseyPainterEditorVectorObjectScaleToolHUD::Reset(FOdysseyVectorScene* iScene)
{
    FOdysseyVectorHUDSelection::Reset( iScene );
}

void
FOdysseyPainterEditorVectorObjectScaleToolHUD::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    BLContext* blctx = iScene->GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    DrawSelectionBox( iScene, iFlags );

    if( mSelectionBox.rect.Area() )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };

        // matrix might get altered for displaying the selection rectangle of a single object. Save it.
        blctx->save();
        blctx->resetMatrix();

        // Top-left handle
        blctx->setFillStyle( ( mHandleFlags & PICK_TOPLEFT     ) ? BLRgba32( 0xFF0000FF )
                                                                 : BLRgba32( 0xFFD0E040 ) );
        blctx->fillCircle( point[0].x, point[0].y, HANDLE_RADIUS );

        // Top-right handle
        blctx->setFillStyle( ( mHandleFlags & PICK_TOPRIGHT    ) ? BLRgba32( 0xFF0000FF )
                                                                 : BLRgba32( 0xFFD0E040 ) );
        blctx->fillCircle( point[1].x, point[1].y, HANDLE_RADIUS );

        // Bottom-right handle
        blctx->setFillStyle( ( mHandleFlags & PICK_BOTTOMRIGHT ) ? BLRgba32( 0xFF0000FF )
                                                                 : BLRgba32( 0xFFD0E040 ) );
        blctx->fillCircle( point[2].x, point[2].y, HANDLE_RADIUS );

        // Bottom-left handle
        blctx->setFillStyle( ( mHandleFlags & PICK_BOTTOMLEFT  ) ? BLRgba32( 0xFF0000FF )
                                                                 : BLRgba32( 0xFFD0E040 ) );
        blctx->fillCircle( point[3].x, point[3].y, HANDLE_RADIUS );

        // Draw a black circle all around the handles
        blctx->setStrokeStyle ( BLRgba32( 0xFF000000 ) );
        blctx->setStrokeWidth ( 1.0f );
        blctx->strokeCircle( point[0].x, point[0].y, HANDLE_RADIUS );
        blctx->strokeCircle( point[1].x, point[1].y, HANDLE_RADIUS );
        blctx->strokeCircle( point[2].x, point[2].y, HANDLE_RADIUS );
        blctx->strokeCircle( point[3].x, point[3].y, HANDLE_RADIUS );

        blctx->restore();
    }
}

uint32
FOdysseyPainterEditorVectorObjectScaleToolHUD::SetCursorPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;

    PickHandle( mX, mY );

    return mHandleFlags;
}

uint32
FOdysseyPainterEditorVectorObjectScaleToolHUD::GetHandleFlags()
{
    return mHandleFlags;
}
