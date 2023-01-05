#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    GetBLContext().end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine(double iWidth,double iHeight)
    : mDrawingFlags( 0 )
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
FOdysseyVectorEngine::RenderHUD()
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    std::list<UOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();

    blctx.save();
    blctx.resetMatrix();

    for( std::list<FOdysseyVectorHUD*>::iterator hit = mHUDList.begin(); hit != mHUDList.end(); ++hit )
    {
        FOdysseyVectorHUD *hud = (*hit);

        for( std::list<UOdysseyVectorObject*>::iterator oit = selectedObjectList.begin(); oit != selectedObjectList.end(); ++oit )
        {
            UOdysseyVectorObject *obj = (*oit);

            hud->Draw( obj, mRoi, 0 );
        }
    }

    blctx.restore();

/*
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    std::list<UOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();

    blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
    blctx.setStrokeWidth(1.0f);

    if( mDrawingFlags & RENDER_OBJECT_BBOX )
    {
        for( std::list<UOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            UOdysseyVectorObject *obj = (*it);
            ::ULIS::FRectD bbox = obj->GetBBox( false );

            blctx.save();
            blctx.setMatrix( obj->GetWorldMatrix() );
            obj->DrawBBox( mRoi, 0 );
            blctx.restore();
        }
    }

    if( mDrawingFlags & RENDER_OBJECT_STRUCTURE )
    {
        for( std::list<UOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            UOdysseyVectorObject *obj = (*it);
            ::ULIS::FRectD bbox = obj->GetBBox( false );

            blctx.save();
            blctx.setMatrix( obj->GetWorldMatrix() );
            obj->DrawStructure( mRoi, 0 );
            blctx.restore();
        }
    }
*/
}

void FOdysseyVectorEngine::SetDrawingFlags( uint64 iDrawingFlags )
{
    mDrawingFlags = iDrawingFlags;
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

    blctx.setFillStyle( BLRgba32(0xFFFFFFFF) );

    if ( mRoi != zeroRectangle )
    {
        blctx.fillRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );

        // view the updated zone ( testing purpose only )
        /*blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
        blctx.setStrokeWidth(1.0f);
        blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
    }
    else
    {
        blctx.fillAll();
    }

    mScene->Draw( mRoi, mDrawingFlags );

    RenderHUD();

    // Reset region of interest after each draw
    memset ( &mRoi, 0, sizeof ( mRoi ) );

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

void
FOdysseyVectorEngine::InvalidateRegion( ::ULIS::FRectI& iRegion )
{
    mRoi.x = iRegion.x;
    mRoi.y = iRegion.y;
    mRoi.w = iRegion.w;
    mRoi.h = iRegion.h;
}

void FOdysseyVectorEngine::AddHUD( FOdysseyVectorHUD* iHUDObject )
{
    mHUDList.push_back( iHUDObject );
}

void FOdysseyVectorEngine::RemoveHUD( FOdysseyVectorHUD* iHUDObject )
{
    mHUDList.remove( iHUDObject );
}

void FOdysseyVectorEngine::ClearHUD()
{
    mHUDList.clear();
}
