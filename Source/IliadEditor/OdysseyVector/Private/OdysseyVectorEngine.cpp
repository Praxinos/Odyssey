// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVector.h"
#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorCell.h"
#include <future>
#include <execution>

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
}

FOdysseyVectorEngine::FOdysseyVectorEngine()
    : mInvalidationFlags( 0 )
{
    // Configure the number of threads to use.
    mProcessorCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
}

uint64
FOdysseyVectorEngine::GetInvalidationFlags()
{
    return mInvalidationFlags;
}

/*
void
FOdysseyVectorEngine::Invalidate( FOdysseyVectorGroupPaint* iScene, uint64 iExtraInvalidationFlags )
{
    uint64 newInvalidationflags = ( mInvalidationFlags | INVALIDATE_DEFAULT | iExtraInvalidationFlags );

    //if( mInvalidationFlags != newInvalidationflags )
    {
        mInvalidationFlags = newInvalidationflags;

        if( iExtraInvalidationFlags & INVALIDATE_CLEAR_ALL )
        {
            mInvalidatedRect = ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
        }

        mOnInvalidateDelegate.Broadcast( iScene, iExtraInvalidationFlags );
    }
}
*/

/*
FOdysseyInvalidTileMap&
FOdysseyVectorEngine::GetInvalidTileMap()
{
    return mInvalidTileMap;
}
*/

BLImageData&
FOdysseyVectorEngine::GetRenderData()
{
    return mRenderData;
}

::ULIS::FRectD
FOdysseyVectorEngine::SanitizeRect( const ::ULIS::FRectD& iRenderRect
                                  , double iScreenWidth
                                  , double iScreenHeight )
{
    ::ULIS::FRectD screen = ::ULIS::FRectD( 0, 0, iScreenWidth, iScreenHeight );
    ::ULIS::FRectD sanitizedRect;
    ::ULIS::FRectD retRect;

    FOdysseyVector::IntersectRegions( iRenderRect
                                    , screen
                                    , &sanitizedRect );

    // note: if the region is 0, we also redraw everything
    retRect = ( iRenderRect.Area() == 0.0f ) ? screen : sanitizedRect;

    // sanitize
    if( retRect.x < 0.0f )
    {
        retRect.x = 0.0f;
    }

    if( retRect.y < 0.0f )
    {
        retRect.y = 0.0f;
    }

    return retRect;
}

void
FOdysseyVectorEngine::RenderHUD( BLContext* iBLContext
                               , FOdysseyVectorGroupPaint* iScene )
{
/*
    FOdysseyVectorGroupPaint* scene = iScene;
    BLImage* image = iBLContext->targetImage();

    // for some unknown reason there was a case where the proxy, at the loading of the file, called this func
    // and image was nullptr. check it.
    if( image == nullptr )
        return;

    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::RenderHUD);

    mDrawingMutex.lock();

    // retrieves buffer specs and allows us to draw directly in the buffer
    image->makeMutable( &mRenderData );

    iBLContext->save();
    iBLContext->resetMatrix();
    //mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    //mBLContext->setFillAlpha( 0.0f );
    iBLContext->clearAll();

    for( IOdysseyVectorHUD *hud : scene->GetCell()->GetHUDList() )
    {
        hud->Draw( iBLContext );
    }

    iBLContext->restore();

    iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

    mRenderData.reset();

    mDrawingMutex.unlock();
*/
}



/*
std::mutex&
FOdysseyVectorEngine::GetDrawingMutex()
{
    return mDrawingMutex;
}
*/

::ULIS::FRectD
FOdysseyVectorEngine::Render( BLContext* iBLContext
                            , const ::ULIS::FRectD& iRedrawRect
                            , FOdysseyVectorGroupPaint* iScene
                            , uint64 iDrawingFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Render);
    BLImage* image = iBLContext->targetImage();
    ::ULIS::FRectD sanitizedRect;
    ::ULIS::FRectD screen;
    FOdysseyVectorGroupPaint* scene = iScene;

    // for some unknown reason there was a case where the proxy, at the loading of the file, called this func
    // and image was nullptr. check it.
    if( image == nullptr )
        return ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0F );

    mDrawingMutex.lock();

    // retrieves buffer specs and allows us to draw directly in the buffer
    image->makeMutable( &mRenderData );

    // we need to get a sanitized version of the rendering region because when we tell the
    //  engine to redraw the whole screen, the region W and H values are set to DBL_MAX
    // via a call to FOdysseyVectorEngine::Invalidate(void). This is because in some
    // portion of the code (the mouse tools, namely) we can't really know the size of the
    // recipient image because we deal with FOdysseyMediaVector which are not supposed to
    // know the size of the recipient image. Only FOdysseyVectorEngine::Render() know that
    // because it gets the BLContext as an argument.
    // So we just set the rectangle to a huge value that basically says "redraw everything"
    // and this is delt with by the Odyssey*ImageRenderers.
    //sanitizedRect = GetInvalidatedRect( iScene->GetCell()->GetInvalidatedRect(), mRenderData.size.w, mRenderData.size.h );

    sanitizedRect = iRedrawRect;

    // for drawing polygones (textured)
    if( mHorizontalLineBuffer.size() != mRenderData.size.h )
    {
        mHorizontalLineBuffer.resize( mRenderData.size.h );
    }

    if( sanitizedRect.Area() )
    {
        BLRgba32 blFillColor;
        FColor fillColor = ( iDrawingFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR ) ? scene->GetMonochromeColor()
                                                                                         : scene->GetBackgroundColor();


        iBLContext->save();
        iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

        blFillColor.setR( fillColor.R );
        blFillColor.setG( fillColor.G );
        blFillColor.setB( fillColor.B );
        blFillColor.setA( fillColor.A );

        iBLContext->setFillStyle( blFillColor );

        iBLContext->clipToRect( BLRect( sanitizedRect.x - 1
                                      , sanitizedRect.y - 1
                                      , sanitizedRect.w + 2
                                      , sanitizedRect.h + 2 ) );
        // Note: we enlarge the block 1 pixel because when FOdysseyVectorBlock renders,
        // the rect seems to be 1 pixel larger. If we don't do this, then the block
        // isn't filled fully and this creates an artefact on the screen.
        iBLContext->fillRect( BLRect( sanitizedRect.x - 1
                                    , sanitizedRect.y - 1
                                    , sanitizedRect.w + 2
                                    , sanitizedRect.h + 2 ) );

        scene->Draw( iBLContext, this, sanitizedRect, 1.0f, iDrawingFlags );

        //--- uncomment to view the invalidation rectangle --- //
        //iBLContext->setStrokeWidth( 2.0f );
        //iBLContext->setStrokeStyle( BLRgba32( 0, 255, 0, 255 ) );
        //iBLContext->strokeRect( BLRect( sanitizedRect.x
        //                              , sanitizedRect.y
        //                              , sanitizedRect.w - 1
        //                              , sanitizedRect.h - 1 ) );
        //------------------------------------------------------//

        iBLContext->restore();

        if( scene->GetLayer() )
        {
            // Because the Proxy can run this function at anytime, we must also protect the access
            // to the list of shared tags
            scene->GetLayer()->GetSharedTagMutex().lock();

            for ( FOdysseyVectorTag* tag : scene->GetLayer()->GetSharedTagList() )
            {
                FOdysseyVectorObject* tagOwner = tag->GetOwner();

                // only draw tag as a shared tag if it does NOT belong to the scene
                if( tagOwner->GetScene() != scene )
                {
                    tagOwner->LockDrawing();
                    tag->Draw( scene, iBLContext, this, sanitizedRect, 1.0f, iDrawingFlags );
                    tagOwner->UnlockDrawing();
                }
            }

            scene->GetLayer()->GetSharedTagMutex().unlock();
        }

        // redraw HUDs that need to be fused with the render.
        scene->GetCell()->LockDrawing();
        for( IOdysseyVectorHUD *hud : scene->GetCell()->GetHUDList() )
        {
            hud->Draw( iBLContext );
        }
        scene->GetCell()->UnlockDrawing();

        iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    }

/*
    if( scene->GetCell()->GetBLMask() )
    {
        iBLContext->blitImage( BLPoint(0,0), *scene->GetCell()->GetBLMask() );
        iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    }
*/
    // reset invalidation region
    // ( refresh the whole screen at next iteration unless this is set
    // to some value ).
    //mInvalidatedRect = ::ULIS::FRectD( 0, 0, 0, 0 );
    // this resets the rect to its default size
    scene->GetCell()->SetPendingRedraw( false );

    mInvalidationFlags = 0;

    mRenderData.reset();

    mDrawingMutex.unlock();

    return sanitizedRect;
}

FOdysseyVectorEngine::FNotifyDelegate&
FOdysseyVectorEngine::OnNotifyDelegate()
{
    static FNotifyDelegate OnNotifyDelegate;

    return OnNotifyDelegate;
}

// static
void
FOdysseyVectorEngine::Notify( FOdysseyVectorGroupPaint* iScene, uint64 iNotifyFlags )
{
    OnNotifyDelegate().Broadcast( iScene, iNotifyFlags );
}

void
FOdysseyVectorEngine::TraceLine ( int32 iX0
                                , int32 iY0
                                , double iU0
                                , double iV0
                                , int32 iX1
                                , int32 iY1
                                , double iU1
                                , double iV1
                                , uint32 iImageWidth
                                , uint32 iImageHeight )
{
    int32 dx  = ( iX1 - iX0 ),
          ddx = abs ( dx ),
          dy  = ( iY1 - iY0 ),
          ddy = abs ( dy ),
          dd  = ( ddx > ddy ) ? ddx : ddy;
    double du  = iU1  - iU0, pu = ( dd ) ? ( du / dd ) : 0.0f;
    double dv  = iV1  - iV0, pv = ( dd ) ? ( dv / dd ) : 0.0f;
    int px = ( dx > 0 ) ? 1 : -1,
        py = ( dy > 0 ) ? 1 : -1;
    int32 x = iX0,
          y = iY0;
    double u = iU0;
    double v = iV0;
    int cumul = 0;
    // we plot only 1 pixel per horizontal line. This is need only when ddx > ddy
    bool pixelOnLine = false;

    if( ddx > ddy )
    {
        for( int i = 0; i <= ddx; i++ )
        {
            if( ( y >= 0 ) && ( y < (int32) iImageHeight ) )
            {
                uint32 offset = ( y * iImageWidth ) + x;

                if( pixelOnLine == false )
                {
                    if( mHorizontalLineBuffer[y].inited == 0 )
                    {
                        mHorizontalLineBuffer[y].inited = 1;
                        mHorizontalLineBuffer[y].y  = y;
                        mHorizontalLineBuffer[y].x0 = mHorizontalLineBuffer[y].x1 = x;
                        mHorizontalLineBuffer[y].u0 = mHorizontalLineBuffer[y].u1 = u;
                        mHorizontalLineBuffer[y].v0 = mHorizontalLineBuffer[y].v1 = v;
                    }
                    else
                    {
                        if( x < mHorizontalLineBuffer[y].x0 )
                        {
                            mHorizontalLineBuffer[y].x0 = x;
                            mHorizontalLineBuffer[y].u0 = u;
                            mHorizontalLineBuffer[y].v0 = v;
                        }

                        if( x > mHorizontalLineBuffer[y].x1 )
                        {
                            mHorizontalLineBuffer[y].x1 = x;
                            mHorizontalLineBuffer[y].u1 = u;
                            mHorizontalLineBuffer[y].v1 = v;
                        }

                        mHorizontalLineBuffer[y].inited = 2;
                    }

                    pixelOnLine = true;
                }
            }

            cumul += ddy;
            x     += px;
            u     += pu;
            v     += pv;

            if( cumul >= ddx )
            {
                cumul -= ddx;
                y     += py;

                pixelOnLine = false;
            }
        }
    }
    else
    {
        for( int i = 0x00; i <= ddy; i++ )
        {
            if( ( y >= 0x00 ) && ( y < (int32) iImageHeight ) )
            {
                uint32 offset = ( y * iImageWidth ) + x;

                /*if( pixelOnLine == false )
                {*/
                    if( mHorizontalLineBuffer[y].inited == 0 )
                    {
                         mHorizontalLineBuffer[y].inited = 1;
                         mHorizontalLineBuffer[y].y  = y;
                         mHorizontalLineBuffer[y].x0 = mHorizontalLineBuffer[y].x1 = x;
                         mHorizontalLineBuffer[y].u0 = mHorizontalLineBuffer[y].u1 = u;
                         mHorizontalLineBuffer[y].v0 = mHorizontalLineBuffer[y].v1 = v;
                    }
                    else
                    {
                        if( x < mHorizontalLineBuffer[y].x0 )
                        {
                            mHorizontalLineBuffer[y].x0 = x;
                            mHorizontalLineBuffer[y].u0 = u;
                            mHorizontalLineBuffer[y].v0 = v;
                        }

                        if( x > mHorizontalLineBuffer[y].x1 )
                        {
                            mHorizontalLineBuffer[y].x1 = x;
                            mHorizontalLineBuffer[y].u1 = u;
                            mHorizontalLineBuffer[y].v1 = v;
                        }

                        mHorizontalLineBuffer[y].inited = 2;
                    }
/*
                    pixelOnLine = true;
                }*/
            }

            cumul += ddx;
            y     += py;
            u     += pu;
            v     += pv;

            if( cumul >= ddy )
            {
                cumul -= ddy;
                x     += px;

                //pixelOnLine = false;
            }
        }
    }
}

// bilinear interpolation version of GETPIXEL. currently unused
// Macro for faster execution. Indeed, an inline function is not guaranteed to be inlined.
#define GETPIXELBF(PIXELS,WIDTH,HEIGHT,BITSPERPIXEL,FLAGS,U,V,R,G,B,A)                                                     \
    switch ( BITSPERPIXEL )                                                                                                \
    {                                                                                                                      \
        case 32 :                                                                                                          \
        {                                                                                                                  \
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4])(PIXELS);                                                \
            double TEXUF = (U) * ( (WIDTH ) - 1 );                                                                         \
            double TEXVF = (V) * ( (HEIGHT) - 1 );                                                                         \
            int32  TEXUI = TEXUF;                                                                                          \
            int32  TEXVI = TEXVF;                                                                                          \
            double  WEIGHTU = TEXUF - TEXUI;                                                                               \
            double  WEIGHTV = TEXVF - TEXVI;                                                                               \
            double  INVWEIGHTU = 1.0f - WEIGHTU;                                                                           \
            double  INVWEIGHTV = 1.0f - WEIGHTV;                                                                           \
            uint32 OFFSETTOPLEFT     = ( TEXVI * (WIDTH) ) + TEXUI                                                         \
                 , OFFSETTOPRIGHT    = OFFSETTOPLEFT + 1                                                                   \
                 , OFFSETBOTTOMRIGHT = OFFSETTOPLEFT + 1 + (WIDTH)                                                         \
                 , OFFSETBOTTOMLEFT  = OFFSETTOPLEFT + (WIDTH);                                                            \
            uint8 UPOL0, UPOL1;                                                                                            \
            uint8 VPOL0, VPOL1;                                                                                            \
                                                                                                                           \
            if( ( (FLAGS) & FPolygonDrawingFlags::BRUSHALPHAONLY ) == 0 )                                                  \
            {                                                                                                              \
                /* bilinear interpolations */                                                                              \
                UPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][0] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETTOPRIGHT   ][0] * WEIGHTU ); \
                UPOL1 = ( PIXELS32[OFFSETBOTTOMLEFT][0] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][0] * WEIGHTU ); \
                VPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][0] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMLEFT ][0] * WEIGHTV ); \
                VPOL1 = ( PIXELS32[OFFSETTOPRIGHT  ][0] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][0] * WEIGHTV ); \
                B = ( ( UPOL0 * ( INVWEIGHTV ) ) + ( UPOL1 * WEIGHTV )                                                     \
                    + ( VPOL0 * ( INVWEIGHTU ) ) + ( VPOL1 * WEIGHTU ) ) * 0.5f;                                           \
                /* bilinear interpolations */                                                                              \
                UPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][1] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETTOPRIGHT   ][1] * WEIGHTU ); \
                UPOL1 = ( PIXELS32[OFFSETBOTTOMLEFT][1] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][1] * WEIGHTU ); \
                VPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][1] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMLEFT ][1] * WEIGHTV ); \
                VPOL1 = ( PIXELS32[OFFSETTOPRIGHT  ][1] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][1] * WEIGHTV ); \
                G = ( ( UPOL0 * ( INVWEIGHTV ) ) + ( UPOL1 * WEIGHTV )                                                     \
                    + ( VPOL0 * ( INVWEIGHTU ) ) + ( VPOL1 * WEIGHTU ) ) * 0.5f;                                           \
                /* bilinear interpolations */                                                                              \
                UPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][2] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETTOPRIGHT   ][2] * WEIGHTU ); \
                UPOL1 = ( PIXELS32[OFFSETBOTTOMLEFT][2] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][2] * WEIGHTU ); \
                VPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][2] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMLEFT ][2] * WEIGHTV ); \
                VPOL1 = ( PIXELS32[OFFSETTOPRIGHT  ][2] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][2] * WEIGHTV ); \
                R = ( ( UPOL0 * ( INVWEIGHTV ) ) + ( UPOL1 * WEIGHTV )                                                     \
                    + ( VPOL0 * ( INVWEIGHTU ) ) + ( VPOL1 * WEIGHTU ) ) * 0.5f;                                           \
            }                                                                                                              \
                                                                                                                           \
            /* bilinear interpolations */                                                                                  \
            UPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][3] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETTOPRIGHT   ][3] * WEIGHTU );     \
            UPOL1 = ( PIXELS32[OFFSETBOTTOMLEFT][3] * ( INVWEIGHTU ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][3] * WEIGHTU );     \
            VPOL0 = ( PIXELS32[OFFSETTOPLEFT   ][3] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMLEFT ][3] * WEIGHTV );     \
            VPOL1 = ( PIXELS32[OFFSETTOPRIGHT  ][3] * ( INVWEIGHTV ) ) + ( PIXELS32[OFFSETBOTTOMRIGHT][3] * WEIGHTV );     \
            A = ( ( UPOL0 * ( INVWEIGHTV ) ) + ( UPOL1 * WEIGHTV )                                                         \
                + ( VPOL0 * ( INVWEIGHTU ) ) + ( VPOL1 * WEIGHTU ) ) * 0.5f;                                               \
        }                                                                                                                  \
        break;                                                                                                             \
                                                                                                                           \
        default :                                                                                                          \
        break;                                                                                                             \
    }                                                                                                                      \

// Macro for faster execution. Indeed, an inline function is not guaranteed to be inlined.
#define GETPIXEL(PIXELS,WIDTH,HEIGHT,BITSPERPIXEL,FLAGS,U,V,R,G,B,A)       \
    switch ( BITSPERPIXEL )                                                \
    {                                                                      \
        case 32 :                                                          \
        {                                                                  \
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4])(PIXELS);\
            double SANU = (U) < 0.0f ? 1.0f + (U) : (U);                   \
            double SANV = (V) < 0.0f ? 1.0f + (V) : (V);                   \
            int32 TEXU = SANU * ( (WIDTH ) - 1 );                          \
            int32 TEXV = SANV * ( (HEIGHT) - 1 );                          \
            uint32 TEXOFFSET = ( TEXV * (WIDTH) ) + TEXU;                  \
                                                                           \
            if( ( (FLAGS) & FPolygonDrawingFlags::BRUSHALPHAONLY ) == 0 )  \
            {                                                              \
                (B) = (PIXELS32)[TEXOFFSET][0];                            \
                (G) = (PIXELS32)[TEXOFFSET][1];                            \
                (R) = (PIXELS32)[TEXOFFSET][2];                            \
            }                                                              \
                                                                           \
            (A) = (PIXELS32)[TEXOFFSET][3];                                \
        }                                                                  \
        break;                                                             \
                                                                           \
        default :                                                          \
        break;                                                             \
    }                                                                      \

/*
struct _EngineTexture
{
    int8*  pixelData;
    uint32 width;
    uint32 height;
    int32  bitsPerPixel;
};

struct _EngineTexture
{

};
*/

static inline void TraceHorizontalLine ( const FHorizontalLine *hline
                                       , double iOpacity
                                       , const int8*  iImagePixelData
                                       , uint32 iImageWidth
                                       , uint32 iImageHeight
                                       , int32  iImageBitsPerPixel
                                       , const  FColor& iColor
                                       //
                                       , const int8*  iBrushPixelData
                                       , uint32 iBrushWidth
                                       , uint32 iBrushHeight
                                       , int32  iBrushBitsPerPixel
                                       , uint64 iPolygonDrawingFlags  )
{
    int32 x0 = hline->x0,
          x1 = hline->x1;
    double u0 = hline->u0;
    double v0 = hline->v0;
    int32 dx = x1 - x0;
    int32 x = x0;
    double du  = hline->u1 - hline->u0, pu = ( dx ) ? ( du / dx ) : 0.0f;
    double dv  = hline->v1 - hline->v0, pv = ( dx ) ? ( dv / dx ) : 0.0f;
    double u = u0;
    double v = v0;
    uint32 offset = ( hline->y * iImageWidth );
    int32 screenx = dx;
    unsigned char BR = iColor.R, BG = iColor.G, BB = iColor.B, BA = iColor.A;

    // Clipping. Note: x1 MUST be > 0, which is checked before the call to this function
    if( x0 < 0 )
    {
        int32 clippingW = -x0;

        x = 0;
        u  += ( clippingW * pu );
        v  += ( clippingW * pv );

        screenx  = dx - clippingW;
    }

    // Replace "<=" with "<" if you don't want to draw from edge-to-edge and stop 1 pixel before to prevent overlapping,
    // which leads to double stroke and would produce artefact when alpha is semi-transparent.
    //for( int i = 0; i <= ddx; i++ )
    for( int i = 0; ( i < screenx ) && ( x < (int)iImageWidth /* clipping */ ); i++ )
    {
        if( ( x >= 0 ) && ( x < (int32) iImageWidth ) )
        {
            uint32 aoffset = offset + x;

            if( iBrushPixelData && iBrushWidth && iBrushHeight )
            {
                double argu = fabs(u) > 1.0f ? u - (int)u : u;
                double argv = fabs(v) > 1.0f ? v - (int)v : v;

                if( ( iPolygonDrawingFlags & FPolygonDrawingFlags::BILINEARFILTERING )
                 && (        x < (int32)(iImageWidth  - 1) )   // prevent overflow
                 && ( hline->y < (int32)(iImageHeight - 1) ) ) // prevent overflow
                {


                    GETPIXELBF( iBrushPixelData
                              , iBrushWidth
                              , iBrushHeight
                              , iBrushBitsPerPixel
                              , iPolygonDrawingFlags
                              , argu
                              , argv
                              , BR
                              , BG
                              , BB
                              , BA );
                }
                else
                {
                    GETPIXEL( iBrushPixelData
                            , iBrushWidth
                            , iBrushHeight
                            , iBrushBitsPerPixel
                            , iPolygonDrawingFlags
                            , argu
                            , argv
                            , BR
                            , BG
                            , BB
                            , BA );
                }
            }

            switch ( iImageBitsPerPixel )
            {
                case 32 :
                {
                    unsigned char (*srcimg)[4] = ( unsigned char (*)[4]) iImagePixelData;

                    if( BA )
                    {
                        // https://en.wikipedia.org/wiki/Alpha_compositing
                        // Premultiplied OVER operation
                        double BAf = ( double ) BA * iOpacity / 255;
                        uint8 B = ( BB * BAf      ) + ( srcimg[aoffset][0] * ( 1.0f - BAf ) );
                        uint8 G = ( BG * BAf      ) + ( srcimg[aoffset][1] * ( 1.0f - BAf ) );
                        uint8 R = ( BR * BAf      ) + ( srcimg[aoffset][2] * ( 1.0f - BAf ) );
                        uint8 A = ( BA * iOpacity ) + ( srcimg[aoffset][3] * ( 1.0f - BAf ) );

                        srcimg[aoffset][0] = B;
                        srcimg[aoffset][1] = G;
                        srcimg[aoffset][2] = R;
                        srcimg[aoffset][3] = A;
                    }
                }
                break;

                default :
                break;
            }
        }

        x ++;
        u += pu;
        v += pv;
    }
}

void
FOdysseyVectorEngine::FillQuad( BLContext* iBLContext
                              , const ::ULIS::FVec2D* iPoint
                              , const double* iU
                              , const double* iV
                              , double iOpacity
                              //
                              , const FColor& iColor
                              //
                              , const int8*  iBrushPixelData
                              , uint32 iBrushWidth
                              , uint32 iBrushHeight
                              , int32  iBrushBitsPerPixel
                              , uint64 iPolygonDrawingFlags )
{
    if( iOpacity )
    {
        const BLMatrix2D& userMatrix = iBLContext->userMatrix();
        BLPoint worldPoint[4] = { userMatrix.mapPoint( iPoint[0].x, iPoint[0].y )
                                , userMatrix.mapPoint( iPoint[1].x, iPoint[1].y )
                                , userMatrix.mapPoint( iPoint[2].x, iPoint[2].y )
                                , userMatrix.mapPoint( iPoint[3].x, iPoint[3].y ) };
        ::ULIS::FVec2I intPt[4] = { { (int32)worldPoint[0].x, (int32)worldPoint[0].y }
                                  , { (int32)worldPoint[1].x, (int32)worldPoint[1].y }
                                  , { (int32)worldPoint[2].x, (int32)worldPoint[2].y }
                                  , { (int32)worldPoint[3].x, (int32)worldPoint[3].y } };
        int32 xmin = intPt[0].x;
        int32 xmax = intPt[0].x;
        int32 ymin = intPt[0].y;
        int32 ymax = intPt[0].y;

        for( int i = 1; i < 4; i++ )
        {
            if( intPt[i].x < xmin ) xmin = intPt[i].x;
            if( intPt[i].x > xmax ) xmax = intPt[i].x;
            if( intPt[i].y < ymin ) ymin = intPt[i].y;
            if( intPt[i].y > ymax ) ymax = intPt[i].y;
        }

        // don't draw if quad is outside the screen
        if( ( ( xmin ) < (int32) mRenderData.size.w )
         && ( ( xmax ) > 0                          )
         && ( ( ymin ) < (int32) mRenderData.size.h )
         && ( ( ymax ) > 0                          ) )
        {
            TracePolygon( intPt
                        , iU
                        , iV
                        , 4
                        , iOpacity
                        , iColor
                        , iBrushPixelData
                        , iBrushWidth
                        , iBrushHeight
                        , iBrushBitsPerPixel
                        , iPolygonDrawingFlags );
        }
    }
}

void
FOdysseyVectorEngine::FillTriangle( BLContext* iBLContext
                                  , const ::ULIS::FVec2D* iPoint
                                  , const double* iU
                                  , const double* iV
                                  , double iOpacity
                                  //
                                  , const FColor& iColor
                                  //
                                  , const int8*  iBrushPixelData
                                  , uint32 iBrushWidth
                                  , uint32 iBrushHeight
                                  , int32  iBrushBitsPerPixel
                                  , uint64 iPolygonDrawingFlags )
{
    if( iOpacity )
    {
        const BLMatrix2D& userMatrix = iBLContext->userMatrix();
        BLPoint worldPoint[3] = { userMatrix.mapPoint( iPoint[0].x, iPoint[0].y )
                                , userMatrix.mapPoint( iPoint[1].x, iPoint[1].y )
                                , userMatrix.mapPoint( iPoint[2].x, iPoint[2].y ) };
        ::ULIS::FVec2I intPt[3] = { { (int32)worldPoint[0].x, (int32)worldPoint[0].y }
                                  , { (int32)worldPoint[1].x, (int32)worldPoint[1].y }
                                  , { (int32)worldPoint[2].x, (int32)worldPoint[2].y } };
        int32 xmin = intPt[0].x;
        int32 xmax = intPt[0].x;
        int32 ymin = intPt[0].y;
        int32 ymax = intPt[0].y;

        for( int i = 1; i < 3; i++ )
        {
            if( intPt[i].x < xmin ) xmin = intPt[i].x;
            if( intPt[i].x > xmax ) xmax = intPt[i].x;
            if( intPt[i].y < ymin ) ymin = intPt[i].y;
            if( intPt[i].y > ymax ) ymax = intPt[i].y;
        }

        // don't draw if quad is outside the screen
        if( ( ( xmin ) < (int32) mRenderData.size.w )
         && ( ( xmax ) > 0                          )
         && ( ( ymin ) < (int32) mRenderData.size.h )
         && ( ( ymax ) > 0                          ) )
        {
            TracePolygon( intPt
                        , iU
                        , iV
                        , 3
                        , iOpacity
                        , iColor
                        , iBrushPixelData
                        , iBrushWidth
                        , iBrushHeight
                        , iBrushBitsPerPixel
                        , iPolygonDrawingFlags );
        }
    }
}

void
FOdysseyVectorEngine::TracePolygon( const ::ULIS::FVec2I* iPoint
                                  , const double* iU
                                  , const double* iV
                                  , uint32 pointCount
                                  , double iOpacity
                                  //
                                  , const FColor& iColor
                                  //
                                  , const int8*  iBrushPixelData
                                  , uint32 iBrushWidth
                                  , uint32 iBrushHeight
                                  , int32  iBrushBitsPerPixel
                                  , uint64 iPolygonDrawingFlags )
{
    int32 ymin = iPoint[0].y,
          ymax = ymin;

    for( uint32 i = 0; i < pointCount; i++ )
    {
        uint32 n = ( i + 1 ) % pointCount;
        double ui = iU ? iU[i] : 0.0f;
        double un = iU ? iU[n] : 0.0f;
        double vi = iV ? iV[i] : 0.0f;
        double vn = iV ? iV[n] : 0.0f;

        if ( iPoint[i].y < ymin ) ymin = iPoint[i].y;
        if ( iPoint[i].y > ymax ) ymax = iPoint[i].y;

        // always draw in the same direction (left to right ) to avoid bad overlapping
        if( iPoint[i].x < iPoint[n].x )
        {
            TraceLine ( iPoint[i].x, iPoint[i].y, ui, vi
                      , iPoint[n].x, iPoint[n].y, un, vn, mRenderData.size.w, mRenderData.size.h );
        }
        else
        {
            TraceLine ( iPoint[n].x, iPoint[n].y, un, vn
                      , iPoint[i].x, iPoint[i].y, ui, vi, mRenderData.size.w, mRenderData.size.h );
        }
    }

    if ( ymin <  0                            ) ymin = 0;
    if ( ymin >= (int32) mRenderData.size.h   ) ymin = (int32) mRenderData.size.h - 1;
    if ( ymax <  0                            ) ymax = 0;
    if ( ymax >= (int32) mRenderData.size.h   ) ymax = (int32) mRenderData.size.h - 1;

    if ( ymin <= ymax )
    {
        // Single CPU version. The one that actually works.
        for ( int i = ymin; i <= ymax; i++ )
        {
            if( mHorizontalLineBuffer[i].inited == 2 )
            {
                mHorizontalLineBuffer[i].inited = 0;

                if( ( mHorizontalLineBuffer[i].x1 >= 0 )
                 && ( mHorizontalLineBuffer[i].x0 < (int32) mRenderData.size.w )
                 // we don't draw the last line to prevent 2 horizontal lines from 2 different polygons
                 // overlapping. this is still experimental, I am not sure this is the right thing to do.
                 && ( i != ymax ) )
                {
                    // this is to prevent overlapping in semi-transparent drawings.
                    // Note: this could be made useless by using a Z-buffer, but we'll save this option for later.
                    //if( ( i == iPoint[0].y )
                    // && ( i == iPoint[3].y )
                    // && ( iPolygonDrawingFlags & FPolygonDrawingFlags::SKIPFIRSTHLINE ) )
                    //{
                    //    continue;
                    //}

                    TraceHorizontalLine( &mHorizontalLineBuffer[i]
                                       , iOpacity
                                       , (int8*)mRenderData.pixelData
                                       , mRenderData.size.w
                                       , mRenderData.size.h
                                       , ( mRenderData.format == BL_FORMAT_PRGB32 ) ? 32 : 0
                                       , iColor
                                       , iBrushPixelData
                                       , iBrushWidth
                                       , iBrushHeight
                                       , iBrushBitsPerPixel
                                       , iPolygonDrawingFlags );
                }
            }
        }
    }
}

// swaps two numbers
static inline void swap(int* a , int*b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

//returns integer part of a floating point number
static inline int iPartOfNumber(float x)
{
    return (int)x;
}

//rounds off a number
static inline int roundNumber(float x)
{
    return iPartOfNumber(x + 0.5) ;
}

//returns fractional part of a number
static inline float fPartOfNumber(float x)
{
    if (x>0) return x - iPartOfNumber(x);
    else return x - (iPartOfNumber(x)+1);
}

//returns 1 - fractional part of number
static inline float rfPartOfNumber(float x)
{
    return 1.0f - fPartOfNumber(x);
}

// draws a pixel on screen of given brightness
// 0<=brightness<=1. We can use your own library
// to draw on screen
static inline void PlotPixel( int32 x
                            , int32 y
                            , double brightness
                            , const int8* iImagePixelData
                            , uint32 iImageWidth
                            , uint32 iImageHeight
                            , int32  iImageBitsPerPixel
                            , const FColor& iColor )
{
    double invBrightness = 1.0f - brightness;

    switch ( iImageBitsPerPixel )
    {
        case 32 :
        {
            unsigned char (*pixels32)[4] = ( unsigned char (*)[4] ) iImagePixelData;
            uint32 offset = ( y * iImageWidth ) + x;
            uint8 R, G, B;

            B = pixels32[offset][0];
            G = pixels32[offset][1];
            R = pixels32[offset][2];

            pixels32[offset][0] = ( B * invBrightness ) + ( iColor.B * brightness );
            pixels32[offset][1] = ( G * invBrightness ) + ( iColor.G * brightness );
            pixels32[offset][2] = ( R * invBrightness ) + ( iColor.R * brightness );
        }
        break;

        default :
        break;
    }
}

// https://www.geeksforgeeks.org/anti-aliased-line-xiaolin-wus-algorithm/
void
FOdysseyVectorEngine::DrawLineAA( int32 x0
                                , int32 y0
                                , int32 x1
                                , int32 y1
                                , const int8* iImagePixelData
                                , uint32 iImageWidth
                                , uint32 iImageHeight
                                , int32  iImageBitsPerPixel
                                , const FColor& iColor )
{
    int steep = fabs( y1 - y0 ) > fabs( x1 - x0 );

    // swap the co-ordinates if slope > 1 or we
    // draw backwards
    if ( steep )
    {
        swap( &x0, &y0 );
        swap( &x1, &y1 );
    }
    if ( x0 > x1 )
    {
        swap( &x0, &x1 );
        swap( &y0, &y1 );
    }

    //compute the slope
    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient = ( dx == 0.0 ) ? 1.0f : ( dy / dx );
    int xpxl1 = x0;
    int xpxl2 = x1;
    float intersectY = y0;

    // main loop
    if ( steep )
    {
        int x;
        for ( x = xpxl1; x <= xpxl2 ; x++ )
        {
            int32 p0x = iPartOfNumber( intersectY );
            int32 p0y = x;
            // there seemed to be a mistake here in the algo borrowed from the mentionned website.
            // Replaced - 1 with + 1, as described in wikipedia
            int32 p1x = p0x + 1;
            int32 p1y = x;

            // pixel coverage is determined by fractional
            // part of y co-ordinate
            if( ( p0x >= 0 ) && ( p0x < (int32) iImageWidth  )
             && ( p0y >= 0 ) && ( p0y < (int32) iImageHeight ) )
            {
                PlotPixel( p0x
                         , p0y
                         , rfPartOfNumber( intersectY )
                         , iImagePixelData
                         , iImageWidth
                         , iImageHeight
                         , iImageBitsPerPixel
                         , iColor );
            }

            if( ( p1x >= 0 ) && ( p1x < (int32) iImageWidth  )
             && ( p1y >= 0 ) && ( p1y < (int32) iImageHeight ) )
            {
                PlotPixel( p1x
                         , p1y
                         , fPartOfNumber ( intersectY )
                         , iImagePixelData
                         , iImageWidth
                         , iImageHeight
                         , iImageBitsPerPixel
                         , iColor );
            }

            intersectY += gradient;
        }
    }
    else
    {
        int x;
        for ( x = xpxl1 ; x <= xpxl2; x++ )
        {
            int32 p0x = x;
            int32 p0y = iPartOfNumber( intersectY );
            int32 p1x = x;
            // there seemed to be a mistake here in the algo borrowed from the mentionned website.
            // Replaced - 1 with + 1, as described in wikipedia
            int32 p1y = p0y + 1;

            // pixel coverage is determined by fractional
            // part of y co-ordinate
            if( ( p0x >= 0 ) && ( p0x < (int32) iImageWidth  )
             && ( p0y >= 0 ) && ( p0y < (int32) iImageHeight ) )
            {
                PlotPixel( p0x
                         , p0y
                         , rfPartOfNumber( intersectY )
                         , iImagePixelData
                         , iImageWidth
                         , iImageHeight
                         , iImageBitsPerPixel
                         , iColor );
            }

            if( ( p1x >= 0 ) && ( p1x < (int32) iImageWidth  )
             && ( p1y >= 0 ) && ( p1y < (int32) iImageHeight ) )
            {
                PlotPixel( p1x
                         , p1y
                         , fPartOfNumber ( intersectY )
                         , iImagePixelData
                         , iImageWidth
                         , iImageHeight
                         , iImageBitsPerPixel
                         , iColor );
            }

            intersectY += gradient;
        }
    }
}

#ifdef unused
void
FOdysseyVectorScene::DrawShape( BLContext* iBLContext, double iCombinedOpacity, uint64 iDrawingFlags )
{
    static ::ULIS::FRectD zeroRectangle; // static variables are always zeroed by default
    BLRgba32 blFillColor;
    FColor fillColor = ( iDrawingFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR ) ? mGroupPaintParam.MonochromeColor
                                                                                     : mBackgroundBucket.GetColor();

    iBLContext->save();

    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

//UE_LOG(LogTemp, Warning, TEXT("Some warning message:%d %d %d %d"), roi.x, roi.y, roi.w, roi.h );

    blFillColor.setR( fillColor.R );
    blFillColor.setG( fillColor.G );
    blFillColor.setB( fillColor.B );
    blFillColor.setA( fillColor.A * iCombinedOpacity );

    iBLContext->setFillStyle( blFillColor );

    //iBLContext->resetMatrix();
    //blctx->clearAll();
    iBLContext->fillAll();

    // TODO: possible optimization: only erase the invalidated part.
    // however, fillRect can do the trick only if the matrix is set to identity.
    //::ULIS::FRectI& rect = GetEngine()->GetInvalidatedRect();
    //iBLContext->fillRect( rect.x, rect.y, rect.w, rect.h );

    FOdysseyVectorGroupPaint::DrawShape( iBLContext, iCombinedOpacity, iDrawingFlags );

    // view the updated zone ( testing purpose only )
    /*iBLContext.setStrokeStyle(BLRgba32(0xFFFF0000));
    iBLContext.setStrokeWidth(1.0f);
    iBLContext.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
    iBLContext->restore();
}


// forbid child removal
uint32
FOdysseyVectorEngine::RemoveChild( FOdysseyVectorObject* iChild )
{
    return FOdysseyVectorObject::HIERARCHY_CHANGE_FORBIDDEN;
}

// forbid child addition
uint32
FOdysseyVectorEngine::AddChild( FOdysseyVectorObject* iChild
                              , FOdysseyVectorObject* iInsertAfter )
{
    return FOdysseyVectorObject::HIERARCHY_CHANGE_FORBIDDEN;
}
#endif // unused
