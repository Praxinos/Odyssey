#include "HUD/OdysseyVectorHUDTransform.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDTransform::~FOdysseyVectorHUDTransform()
{
}

FOdysseyVectorHUDTransform::FOdysseyVectorHUDTransform()
{

}
/*
void
FOdysseyVectorHUDTransform::SetSize( ::ULIS::FRectD mRect )
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
/*
int32
FOdysseyVectorHUDTransform::Pick( double iWorldX, double iWorldY )
{
    BLPoint localCoords = mTransformationSpace.GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );
    BLPoint vec = mTransformationSpace.GetInverseWorldMatrix().mapVector( 1.0f, 1.0f );
    ::ULIS::FVec2D size = { vec.x, vec.y };
    ::ULIS::FRectD handleRect[4];

    MakeHandlesRect ( handleRect, mHandle, mHandleRadius, size.x, size.y );

    // pick the handles
    for ( int i = 0; i < 4; i++ )
    {
        ::ULIS::FVec2D pt = { localCoords.x, localCoords.y };
 
        if ( handleRect[i].HitTest( pt ) == true )
        {
            return i;
        }
    }

    return -1;
}
*/
void
FOdysseyVectorHUDTransform::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
/*
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    BLPoint vec = mTransformationSpace.GetInverseWorldMatrix().mapVector( 1.0f, 1.0f );
    ::ULIS::FVec2D size = { vec.x, vec.y };
    ::ULIS::FRectD handleRect[4];

    MakeHandlesRect ( handleRect, mHandle, mHandleRadius, size.x, size.y );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->setMatrix( mTransformationSpace.GetWorldMatrix() );

    blctx->setStrokeStyle( BLRgba32( 0xFF8B0000 ) );

    blctx->setStrokeWidth( vec.y );
    blctx->strokeLine( mHandle[0].x, mHandle[0].y, mHandle[1].x, mHandle[1].y );
    blctx->strokeLine( mHandle[3].x, mHandle[3].y, mHandle[2].x, mHandle[2].y );

    blctx->setStrokeWidth( vec.x );
    blctx->strokeLine( mHandle[1].x, mHandle[1].y, mHandle[2].x, mHandle[2].y );
    blctx->strokeLine( mHandle[3].x, mHandle[3].y, mHandle[0].x, mHandle[0].y );

    blctx->setFillStyle( BLRgba32( 0xFF8B0000 ) );

    // draw the handles (squares at rectangle corners)
    for ( int i = 0; i < 4; i++ )
    {
        blctx->fillRect( handleRect[i].x, handleRect[i].y, handleRect[i].w, handleRect[i].h );
    }

    blctx->restore();
*/
}
