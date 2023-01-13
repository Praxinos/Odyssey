#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    GetBLContext().end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine( double iWidth, double iHeight )
    : mDrawingFlags( 0 )
{
    mBLImage = new BLImage( iWidth, iHeight, BL_FORMAT_PRGB32 );
    mBLMask  = new BLImage( iWidth, iHeight, BL_FORMAT_A8 );

    mScene = NewObject<UOdysseyVectorRoot>();
    mScene->Init("Vector Scene");

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
        //blctx.clearAll();
    }

    mScene->Draw( mRoi, mDrawingFlags );

    RenderHUD();

/*
    BLImageData imgData;
    BLImageData mskData;

    mBLMask->getData( &mskData );
    mBLImage->getData( &imgData );

    for ( int i = 0; i < imgData.size.h; i++ ) {
        for ( int j = 0; j < imgData.size.w; j++ ) {
            uint32 offset = (i*imgData.size.w)+j;

            if( ((uint8*)mskData.pixelData)[offset] )
                ((uint32*)imgData.pixelData)[offset] = 0xFFFF0000;

        }
    }
*/

    // Reset region of interest after each draw
    memset ( &mRoi, 0, sizeof ( mRoi ) );

    blctx.flush(BL_CONTEXT_FLUSH_SYNC);
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateMask( std::vector<::ULIS::FVec2D>& iPointArray )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPath path;
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };

    /*blctx.setFillStyle( BLRgba32(0x00000000) );*/
    blctx.setFillAlpha(0.0f);
    blctx.clearAll();

    if( iPointArray.size() )
    {
        double x1 = iPointArray[0].x, y1 = iPointArray[0].y
             , x2 = iPointArray[0].x, y2 = iPointArray[0].y;

        path.moveTo( iPointArray[0].x, iPointArray[0].y );

        for( uint32 i = 1; i < iPointArray.size(); i++ )
        {
            path.lineTo( iPointArray[i].x, iPointArray[i].y );

            if( iPointArray[i].x < x1 )
            {
                x1 = iPointArray[i].x;
            }

            if( iPointArray[i].y < y1 )
            {
                y1 = iPointArray[i].y;
            }

            if( iPointArray[i].x > x2 )
            {
                x2 = iPointArray[i].x;
            }

            if( iPointArray[i].y > y2 )
            {
                y2 = iPointArray[i].y;
            }
        }

        rect = ::ULIS::FRectD::FromMinMax( x1, y1, x2, y2 );
    }

    /*blctx.setFillStyle( BLRgba32(0xFFFFFFFF) );*/
    blctx.setFillAlpha(1.0f);
    blctx.fillPath( path );

    return rect;
}

void
FOdysseyVectorEngine::Pick( std::vector<::ULIS::FVec2D>& iPointArray, uint32 iSelectionFlags )
{
    UOdysseyVectorObject* pickedObject;
    ::ULIS::FRectD roi;

    GetBLContext().end();
    GetBLContext().begin( *mBLMask );

    roi = GenerateMask( iPointArray );

    GetBLContext().flush(BL_CONTEXT_FLUSH_SYNC);

    pickedObject = UOdysseyVectorRoot::RecursivePick( *mScene, roi, iSelectionFlags );

    // deselect all
    mScene->ClearSelection();

    if ( pickedObject )
    {
        if( pickedObject->GetClass() == UOdysseyVectorLoop::StaticClass() )
        {
            pickedObject = pickedObject->GetParent();
        }

        mScene->Select ( *pickedObject );
    }

    GetBLContext().end();
    GetBLContext().begin(*mBLImage);
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

FArchive&
operator<<(FArchive &Ar, FOdysseyVectorEngine* iVectorEngine )
{
    UE_LOG(LogTemp,Warning,TEXT("Saving engine"));

    /*iVectorEngine->GetScene()->Serialize(Ar);*/
/*
    Ar << Cast<UOdysseyVectorObject>(iVectorEngine->GetScene());
*/

    return Ar;
}
