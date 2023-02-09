#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    mBLContext->end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine( double iWidth, double iHeight )
    : mDrawingFlags( 0 )
{
    mBLContext = new BLContext();
    mBLImage = new BLImage( iWidth, iHeight, BL_FORMAT_PRGB32 );
    mBLMask  = new BLImage( iWidth, iHeight, BL_FORMAT_A8 );

    /*mScene = NewObject<UOdysseyVectorRoot>();
    mScene->Init("Vector Scene");*/

    mBLContext->begin( *mBLImage );
}

BLContext*
FOdysseyVectorEngine::GetBLContext()
{
    return mBLContext;
}

BLImage*
FOdysseyVectorEngine::GetBLImage()
{
    return mBLImage;
}

BLImage*
FOdysseyVectorEngine::GetBLMask()
{
    return mBLMask;
}

::ULIS::FRectD&
FOdysseyVectorEngine::GetInvalidateRegion()
{
    return mRoi;
}

void
FOdysseyVectorEngine::RenderHUD( UOdysseyVectorRoot& iScene )
{
    std::list<UOdysseyVectorObject*> selectedObjectList = iScene.GetSelectedObjectList();

    mBLContext->save();
    mBLContext->resetMatrix();

    for( std::list<FOdysseyVectorHUD*>::iterator hit = mHUDList.begin(); hit != mHUDList.end(); ++hit )
    {
        FOdysseyVectorHUD *hud = (*hit);

            hud->Draw( iScene, mRoi, 0 );
    }

    mBLContext->restore();

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

void
FOdysseyVectorEngine::SetDrawingFlags( uint64 iDrawingFlags )
{
    mDrawingFlags = iDrawingFlags;
}

void
FOdysseyVectorEngine::Render( UOdysseyVectorRoot& iScene )
{
    static ::ULIS::FRectD zeroRectangle;
    // Blend2D part
   /* BLContextCreateInfo createInfo{};*/

    // Configure the number of threads to use.
    /*createInfo.threadCount = 1;*/

    mBLContext->setFillStyle( BLRgba32(0xFFFFFFFF) );

    if ( mRoi != zeroRectangle )
    {
        mBLContext->fillRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );

        // view the updated zone ( testing purpose only )
        /*blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
        blctx.setStrokeWidth(1.0f);
        blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
    }
    else
    {
        mBLContext->fillAll();
        //blctx.clearAll();
    }

    iScene.Draw( mRoi, mDrawingFlags );

    RenderHUD( iScene );

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

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateMask( std::vector<::ULIS::FVec2D>& iPointArray )
{
    BLPath path;
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };

    /*blctx.setFillStyle( BLRgba32(0x00000000) );*/
    mBLContext->setFillAlpha(0.0f);
    mBLContext->clearAll();

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
    mBLContext->setFillAlpha(1.0f);
    mBLContext->fillPath( path );

    return rect;
}

void
FOdysseyVectorEngine::RecursiveErase( UOdysseyVectorObject& iObj
                                    , std::vector<UOdysseyVectorObject*>& iErasedObjectArray
                                    , ::ULIS::FRectD &iRoi
                                    , bool iSelectedOnly )
{
    for( std::list<UOdysseyVectorObject*>::iterator it = iObj.GetChildrenList().begin(); it != iObj.GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);

        RecursiveErase( *child, iErasedObjectArray, iRoi, iSelectedOnly );
    }

    if( iSelectedOnly == true )
    {
        if( iObj.IsSelected() == true )
        {
            if( iObj.Erase( iRoi ) )
            {
                iErasedObjectArray.push_back( &iObj );
            }
        }
    }
    else
    {
        if( iObj.Erase( iRoi ) )
        {
            iErasedObjectArray.push_back( &iObj );
        }
    }
}

void
FOdysseyVectorEngine::Erase( UOdysseyVectorRoot& iScene
                           , ::ULIS::FRectD &iRoi
                           , bool iSelectedOnly )
{
    std::vector<UOdysseyVectorObject*> erasedObjectArray;

    RecursiveErase( iScene, erasedObjectArray, iRoi, iSelectedOnly  );

    // Note: this will be refactored in case a child is erased and a parent should as well be erased. We'll see.
    for( int i = 0; i < erasedObjectArray.size(); i++ )
    {
        if( erasedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            erasedObjectArray[i]->GetParent()->RemoveChild( erasedObjectArray[i] );
        }
    }
}

// static
void
FOdysseyVectorEngine::RecursivePick( UOdysseyVectorObject& iObj
                                   , std::vector<UOdysseyVectorObject*>& iSelectedObjectArray
                                   , ::ULIS::FRectD& iRoi
                                   , uint32 iSelectionFlags )
{
    UOdysseyVectorObject* pickedObject = iObj.Pick( iRoi, iSelectionFlags );

    for( std::list<UOdysseyVectorObject*>::iterator it = iObj.GetChildrenList().begin(); it != iObj.GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject* child = (*it);

        RecursivePick( *child, iSelectedObjectArray, iRoi, iSelectionFlags );
    }

    if( pickedObject )
    {
        iSelectedObjectArray.push_back( pickedObject );
    }
}

void
FOdysseyVectorEngine::UseMaskImage()
{
    mBLContext->end();
    mBLContext->begin( *mBLMask );
}

void
FOdysseyVectorEngine::UseColorImage()
{
    mBLContext->end();
    mBLContext->begin( *mBLImage );
}

void
FOdysseyVectorEngine::Pick( UOdysseyVectorRoot& iScene, std::vector<::ULIS::FVec2D>& iPointArray, uint32 iSelectionFlags )
{
    ::ULIS::FRectD roi;
    std::vector<UOdysseyVectorObject*> pickedObjectArray;

    UseMaskImage();

    roi = GenerateMask( iPointArray );

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

    // deselect all
    iScene.ClearSelection();

    RecursivePick( iScene, pickedObjectArray, roi, iSelectionFlags );

    for ( int i = 0; i < pickedObjectArray.size(); i++ )
    {
        iScene.Select( pickedObjectArray[i] );
    }

    UseColorImage();
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
