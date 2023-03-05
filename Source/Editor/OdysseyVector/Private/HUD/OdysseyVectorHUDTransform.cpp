#include "HUD/OdysseyVectorHUDTransform.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDTransform::~FOdysseyVectorHUDTransform()
{
}

FOdysseyVectorHUDTransform::FOdysseyVectorHUDTransform()
    : FOdysseyVectorHUDSelection()
{

}

/*
void
FOdysseyVectorHUDTransform::Init( double iX, double iX, double )
{
    mHandle[0].x = mRect.x;
    mHandle[0].y = mRect.y;

    mHandle[1].x = mRect.x + mRect.w;
    mHandle[1].y = mRect.y;

    mHandle[2].x = mRect.x + mRect.w;
    mHandle[2].y = mRect.y + mRect.h;

    mHandle[3].x = mRect.x;
    mHandle[3].y = mRect.y + mRect.h;
}
*/
/*
static void
MakeHandlesRect( ::ULIS::FRectD iRect[4], ::ULIS::FVec2D iHandle[4], double iRadius, double iXFactor, double iYFactor )
{
    double radiusX = iRadius * iXFactor;
    double radiusY = iRadius * iYFactor;
    double width  = radiusX * 2.0f;
    double height = radiusY * 2.0f;

    iRect[0] = ::ULIS::FRectD::FromXYWH( iHandle[0].x - radiusX, iHandle[0].y - radiusY, width, height );
    iRect[1] = ::ULIS::FRectD::FromXYWH( iHandle[1].x - radiusX, iHandle[1].y - radiusY, width, height );
    iRect[2] = ::ULIS::FRectD::FromXYWH( iHandle[2].x - radiusX, iHandle[2].y - radiusY, width, height );
    iRect[3] = ::ULIS::FRectD::FromXYWH( iHandle[3].x - radiusX, iHandle[3].y - radiusY, width, height );
}
*/

int32
FOdysseyVectorHUDTransform::Pick( double iWorldX, double iWorldY )
{
    if( mSelectionBox.space )
    {
        BLPoint localCoords = mSelectionBox.space->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );

        // pick the handles
        for ( int i = 0; i < 4; i++ )
        {
            ::ULIS::FVec2D pt = { localCoords.x, localCoords.y };
 
            if ( mHandle[i].HitTest( pt ) == true )
            {
                return i;
            }
        }
    }

    return -1;
}

static void
MakeHandle( double iLocalX, double iLocalY, double iFactorX, double iFactorY, ::ULIS::FRectD& oRect )
{
    oRect.x = iLocalX - ( FOdysseyVectorHUDTransform::HANDLE_RADIUS * iFactorX );
    oRect.y = iLocalY - ( FOdysseyVectorHUDTransform::HANDLE_RADIUS * iFactorY );
    oRect.w = ( FOdysseyVectorHUDTransform::HANDLE_RADIUS * 2 * iFactorX );
    oRect.h = ( FOdysseyVectorHUDTransform::HANDLE_RADIUS * 2 * iFactorY );
}

void
FOdysseyVectorHUDTransform::UpdateSelectionBox( UOdysseyVectorRoot& iScene )
{

    FOdysseyVectorHUDSelection::UpdateSelectionBox( iScene );

    if( mSelectionBox.space )
    {
        BLPoint size = mSelectionBox.space->GetInverseWorldMatrix().mapVector( HANDLE_RADIUS, HANDLE_RADIUS );

        MakeHandle( mSelectionBox.rect.x                       , mSelectionBox.rect.y                       , size.x, size.y, mHandle[0] );
        MakeHandle( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                       , size.x, size.y, mHandle[1] );
        MakeHandle( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h, size.x, size.y, mHandle[2] );
        MakeHandle( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h, size.x, size.y, mHandle[3] );
    }
}

void
FOdysseyVectorHUDTransform::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    std::list<UOdysseyVectorObject*>& selectedObjectList = iScene.GetSelectedObjectList();
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };

    DrawSelectionBox( iScene, iRoi, iFlags );

    if( mSelectionBox.space )
    {
        // matrix might get altered for displaying the selection rectangle of a single object. Save it.
        blctx->save();
        blctx->setMatrix( mSelectionBox.space->GetWorldMatrix() );

        blctx->setFillStyle( BLRgba32( 0xFF0000FF ) );

        blctx->fillRect( mHandle[0].x, mHandle[0].y, mHandle[0].w, mHandle[0].h );
        blctx->fillRect( mHandle[1].x, mHandle[1].y, mHandle[1].w, mHandle[1].h );
        blctx->fillRect( mHandle[2].x, mHandle[2].y, mHandle[2].w, mHandle[2].h );
        blctx->fillRect( mHandle[3].x, mHandle[3].y, mHandle[3].w, mHandle[3].h );

        blctx->restore();
    }
}
