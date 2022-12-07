#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    GetBLContext().end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine( double iWidth, double iHeight )
    : mScene ( "Vector Scene" )
{
    mBLImage = new BLImage( iWidth, iHeight, BL_FORMAT_PRGB32 );

    GetBLContext().begin( *mBLImage );
}

BLImage&
FOdysseyVectorEngine::GetBLImage()
{
    return *mBLImage;
}

BLContext&
FOdysseyVectorEngine::GetBLContext()
{
    static BLContext* blctx;

    if ( blctx == nullptr )
    {
        blctx = new BLContext();
    }

    return *blctx;
}

::ULIS::FRectD&
FOdysseyVectorEngine::GetInvalidateRegion()
{
    return mRoi;
}

void
FOdysseyVectorEngine::Render( ::ULIS::FBlock& iBlock )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    static ::ULIS::FRectD zeroRectangle;
    // Blend2D part
   /* BLContextCreateInfo createInfo{};*/

    // Configure the number of threads to use.
    /*createInfo.threadCount = 1;*/

    blctx.setFillStyle(BLRgba32(0xFFFFFFFF));

    if ( mRoi != zeroRectangle )
    {
        blctx.fillRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );

        blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
        blctx.setStrokeWidth(1.0f);
        blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );

    }
    else
    {
        blctx.fillAll();
    }

    mScene.Draw( mRoi, 0 );

    // Reset region of interest after each draw
    memset ( &mRoi, 0, sizeof ( mRoi ) );

    blctx.flush( BL_CONTEXT_FLUSH_SYNC );
}

FOdysseyVectorRoot&
FOdysseyVectorEngine::GetScene()
{
    return mScene;
}

void
FOdysseyVectorEngine::InvalidateRegion( double x, double y, double w, double h )
{
    mRoi.x = x;
    mRoi.y = y;
    mRoi.w = w;
    mRoi.h = h;
}

void
FOdysseyVectorEngine::InvalidateRegion( ::ULIS::FRectD& iRegion )
{
    mRoi.x = iRegion.x;
    mRoi.y = iRegion.y;
    mRoi.w = iRegion.w;
    mRoi.h = iRegion.h;
}
