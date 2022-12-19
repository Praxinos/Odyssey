#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    GetBLContext().end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine(double iWidth,double iHeight)
{
    mBLImage = new BLImage(iWidth,iHeight,BL_FORMAT_PRGB32);

    mScene = NewObject<UOdysseyVectorRoot>();
    mScene->Init("Vector Scene");

    GetBLContext().begin(*mBLImage);
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

    if(blctx == nullptr)
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
FOdysseyVectorEngine::RenderSelected()
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    std::list<UOdysseyVectorObject*> selectObjectList = mScene->GetSelectedObjectList();

    blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
    blctx.setStrokeWidth(1.0f);

    for( std::list<UOdysseyVectorObject*>::iterator it = selectObjectList.begin(); it != selectObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);
        ::ULIS::FRectD bbox = obj->GetBBox( false );

        blctx.save();
        blctx.setMatrix( obj->GetWorldMatrix() );
        blctx.strokeRect( bbox.x, bbox.y, bbox.w, bbox.h );
        blctx.restore();
    }
}

void
FOdysseyVectorEngine::Render()
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

        // view the updated zone ( testing purpose only )
        blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
        blctx.setStrokeWidth(1.0f);
        blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );
    }
    else
    {
        blctx.fillAll();
    }

    mScene->Draw(mRoi,0);

    RenderSelected();

    // Reset region of interest after each draw
    memset (&mRoi,0,sizeof (mRoi));

    blctx.flush(BL_CONTEXT_FLUSH_SYNC);
}

UOdysseyVectorRoot*
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
