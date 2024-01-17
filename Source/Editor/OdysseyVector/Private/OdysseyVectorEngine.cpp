#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPath.h"
#include <future>
#include <execution>

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
}

FOdysseyVectorEngine::FOdysseyVectorEngine( FOdysseyVectorGroupPaint* iScene
                                          , uint32 iPreferredWidth
                                          , uint32 iPreferredHeight )
    : FOdysseyVectorObject( "Engine" )
    , mSelectionSpace( nullptr )
    , mInvalidTileMap( 64, iPreferredWidth, iPreferredHeight )
    , mPreferredWidth( iPreferredWidth )
    , mPreferredHeight( iPreferredHeight )
    , mInvalidatedRect( 0, 0, iPreferredWidth, iPreferredHeight )
{
    // Configure the number of threads to use.
    mProcessorCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

    SetScene( iScene );
}

bool
FOdysseyVectorEngine::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorEngine::SetSelectionSpace( FOdysseyVectorGroup* iSelectionSpace )
{
   mSelectionSpace = iSelectionSpace;
}

FOdysseyVectorGroup*
FOdysseyVectorEngine::GetSelectionSpace()
{
    return mSelectionSpace;
}

FULISInvalidTileMap&
FOdysseyVectorEngine::GetInvalidTileMap()
{
    return mInvalidTileMap;
}

BLImage*
FOdysseyVectorEngine::GetBLMask()
{
    return mBLMask;
}

void
FOdysseyVectorEngine::SetBLMask( BLImage* iBLMask )
{
    mBLMask = iBLMask;
}

uint32
FOdysseyVectorEngine::GetPreferredWidth()
{
    return mPreferredWidth;
}

uint32
FOdysseyVectorEngine::GetPreferredHeight()
{
    return mPreferredHeight;
}

BLImageData&
FOdysseyVectorEngine::GetRenderData()
{
    return mRenderData;
}

void
FOdysseyVectorEngine::SetScene( FOdysseyVectorGroupPaint* iScene )
{
    mScene = iScene;

    // We dont use AddChild or RemoveChild because they are
    // overlodaded to prevent manual addition or removal of child objects.
    mScene->SetParent( this );
    mChildrenList.clear();
    mInvalidatedChildrenList.clear();
    mChildrenList.push_back( iScene );
    mSelectedObjectList.clear();

    ResetHUD();

    mScene->UpdateMatrix();
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
}

FOdysseyVectorGroupPaint*
FOdysseyVectorEngine::GetScene()
{
    return mScene;
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorEngine::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

FOdysseyVectorObject*
FOdysseyVectorEngine::GetLastSelectedObject()
{
    return ( mSelectedObjectList.empty() == true ) ? nullptr : mSelectedObjectList.back();
}

void
FOdysseyVectorEngine::ClearObjectSelection()
{
    for( FOdysseyVectorObject *obj : mSelectedObjectList )
    {
        obj->SetSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorEngine::UnselectObject( FOdysseyVectorObject* iVecObj )
{
    iVecObj->SetSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
FOdysseyVectorEngine::SelectObject( FOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

void
FOdysseyVectorEngine::SetInvalidatedRect( const ::ULIS::FRectD& iRect )
{
    ::ULIS::FRectI rect = ::ULIS::FRectI( iRect.x, iRect.y, iRect.w, iRect.h );

    SetInvalidatedRect( rect );
}

void
FOdysseyVectorEngine::SetInvalidatedRect( const ::ULIS::FRectI& iRect )
{
    if( mInvalidationFlags == 0 )
    {
        mInvalidatedRect.x = iRect.x;
        mInvalidatedRect.y = iRect.y;
        mInvalidatedRect.w = iRect.w;
        mInvalidatedRect.h = iRect.h;
    }
    else // if we haven' been redrawn yet, combine the rectangles
    {
        mInvalidatedRect = mInvalidatedRect | iRect;
    }

    // sanitize
    if( mInvalidatedRect.x < 0.0f )
    {
        mInvalidatedRect.x = 0.0f;
    }

    if( mInvalidatedRect.y < 0.0f )
    {
        mInvalidatedRect.y = 0.0f;
    }
}

::ULIS::FRectI&
FOdysseyVectorEngine::GetInvalidatedRect()
{
    return mInvalidatedRect;
}

void
FOdysseyVectorEngine::RenderHUD( BLContext* iBLContext )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::RenderHUD);

    iBLContext->save();
    iBLContext->resetMatrix();
    //mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    //mBLContext->setFillAlpha( 0.0f );
    iBLContext->clearAll();

    for( FOdysseyVectorHUD *hud : GetHUDList() )
    {
        hud->Draw( iBLContext, mScene );
    }

    iBLContext->restore();

    iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
}

void
FOdysseyVectorEngine::SelectAllInSelectionSpace()
{
    // TODO: set scene as the default selection space
    FOdysseyVectorGroup* selectionSpace = mSelectionSpace ? mSelectionSpace : mScene;

    mScene->GetEngine()->ClearObjectSelection();

    for( FOdysseyVectorObject *child : selectionSpace->GetChildrenList() )
    {
        mScene->GetEngine()->SelectObject( child );
    }
}

void
FOdysseyVectorEngine::Render( BLContext* iBLContext, uint64 iDrawingFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Render);
    BLImage* image = iBLContext->targetImage();

    // retrieves uffer specs and allows us to draw directly in the buffer
    image->makeMutable( &mRenderData );

    // for drawing polygones (textured)
    if( mHorizontalLineBuffer.size() != mRenderData.size.h )
    {
        mHorizontalLineBuffer.resize( mRenderData.size.h );
    }

    if( mInvalidationFlags )
    {
        BLRgba32 blFillColor;
        FColor fillColor = ( iDrawingFlags & FOdysseyVectorEngine::DRAWING_IGNORECOLOR ) ? mScene->GetMonochromeColor()
                                                                                         : mScene->GetBackgroundColor();


        iBLContext->save();
        iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

        blFillColor.setR( fillColor.R );
        blFillColor.setG( fillColor.G );
        blFillColor.setB( fillColor.B );
        blFillColor.setA( fillColor.A );

        iBLContext->setFillStyle( blFillColor );

        iBLContext->fillAll();

        mScene->Draw( iBLContext, 1.0f, iDrawingFlags );

        iBLContext->restore();

        iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    }

    mInvalidationFlags = 0;

    mInvalidatedRect = ::ULIS::FRectI( 0, 0, mRenderData.size.w, mRenderData.size.h );

    mRenderData.reset();
}

FOdysseyVectorVertex*
FOdysseyVectorEngine::Stitch( FOdysseyVectorVertex* iVertexA
                            , FOdysseyVectorVertex* iVertexB
                            , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                            , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                            , bool iSmooth )
{
    if( ( iVertexA->GetSegmentCount() == 1 )
     && ( iVertexB->GetSegmentCount() == 1 )
     && ( iVertexA->GetPath() == iVertexB->GetPath() ) )
    {
        ::ULIS::FVec2D& vertexACoords = iVertexA->GetCoords();
        ::ULIS::FVec2D& vertexBCoords = iVertexB->GetCoords();
        ::ULIS::FVec2D averageCoords = ( vertexACoords + vertexBCoords ) * 0.5f;
        double vertexARadius = iVertexA->GetRadius();
        double vertexBRadius = iVertexB->GetRadius();
        double averageRadius = ( vertexARadius + vertexBRadius ) * 0.5f;
        FOdysseyVectorPath* path = iVertexA->GetPath();

        if( path->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* cubicPath = static_cast<FOdysseyVectorPath*>(path);
            FOdysseyVectorSegmentCubic* vertexBSegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexB->GetFirstSegment());
            FOdysseyVectorSegmentCubic* vertexASegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexA->GetFirstSegment());
            FOdysseyVectorVertex* prevVertex = static_cast<FOdysseyVectorVertex*>(vertexASegment->GetOtherVertex( iVertexA ));
            FOdysseyVectorVertex* nextVertex = static_cast<FOdysseyVectorVertex*>(vertexBSegment->GetOtherVertex( iVertexB ));
            FOdysseyVectorVertex* knotVertex = new FOdysseyVectorVertex( averageCoords.x, averageCoords.y, averageRadius );
            FOdysseyVectorSegmentCubic* newCubicSegment[2] = { new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  prevVertex
                                                                                            ,  knotVertex
                                                                                            ,  true ),
                                                               new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  knotVertex
                                                                                            ,  nextVertex
                                                                                            ,  true ) };

            newCubicSegment[0]->GetHandle(0)->Set( vertexASegment->GetHandle(prevVertex)->GetCoords() );
            newCubicSegment[0]->GetHandle(1)->Set( vertexASegment->GetHandle(iVertexA  )->GetCoords() );
            newCubicSegment[1]->GetHandle(0)->Set( vertexBSegment->GetHandle(iVertexB  )->GetCoords() );
            newCubicSegment[1]->GetHandle(1)->Set( vertexBSegment->GetHandle(nextVertex)->GetCoords() );

            path->RemoveSegment( vertexASegment );
            path->RemoveSegment( vertexBSegment );
            path->RemoveVertex( iVertexA );
            path->RemoveVertex( iVertexB );

            oRemovedSegmentArray.push_back( vertexASegment );
            oRemovedSegmentArray.push_back( vertexBSegment );

            path->AddVertex( knotVertex );
            path->AddSegment( newCubicSegment[0] );
            path->AddSegment( newCubicSegment[1] );

            oAddedSegmentArray.push_back( newCubicSegment[0] );
            oAddedSegmentArray.push_back( newCubicSegment[1] );
/*
            if( iSmooth )
            {
                FOdysseyVectorPath::SmoothSegments( knotVertex, false, true );
            }
*/
            path->InvalidateAllSegments();

            //mScene->Update( 0 );

            return knotVertex;
        }
    }

    return nullptr;
}

// static
void
FOdysseyVectorEngine::RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                                   , FOdysseyVectorObject* iObj
                                   , std::vector<FOdysseyVectorObject*>& oSelectedObjectArray
                                   , const ::ULIS::FRectD& iRoi
                                   , uint32 iSelectionFlags )
{
    FOdysseyVectorObject* pickedObject = ( iObj != iSelectionSpace ) ? iObj->Pick( iSelectionSpace, iRoi, iSelectionFlags ) : nullptr;

    for( FOdysseyVectorObject* child : iObj->GetChildrenList() )
    {
        RecursivePick( iSelectionSpace, child, oSelectedObjectArray, iRoi, iSelectionFlags );
    }

    if( pickedObject )
    {
        oSelectedObjectArray.push_back( pickedObject );
    }
}

void
FOdysseyVectorEngine::Pick( FOdysseyVectorGroupPaint* iScene
                          , const ::ULIS::FRectD& iRoi
                          , std::vector<FOdysseyVectorObject*>& oPickedObjectArray
                          , uint32 iSelectionFlags )
{
    RecursivePick( mSelectionSpace ? mSelectionSpace : iScene, iScene, oPickedObjectArray, iRoi, iSelectionFlags );
/*
    if( iSelectionFlags & FOdysseyVectorObject::PICK_MASK_BASED )
    {
        UseColorImage();
    }
*/
}

std::list<FOdysseyVectorHUD*>&
FOdysseyVectorEngine::GetHUDList()
{
    return mHUDList;
}

void
FOdysseyVectorEngine::AddHUD( FOdysseyVectorHUD* iHUDObject )
{
    GetHUDList().push_back( iHUDObject );
}

void
FOdysseyVectorEngine::RemoveHUD( FOdysseyVectorHUD* iHUDObject )
{
    GetHUDList().remove( iHUDObject );
}

void
FOdysseyVectorEngine::ClearHUD()
{
    GetHUDList().clear();
}

void
FOdysseyVectorEngine::ResetHUD()
{
    for( FOdysseyVectorHUD *hud : GetHUDList() )
    {
        hud->Reset( mScene );
    }
}

FOdysseyVectorEngine::FSignalDelegate&
FOdysseyVectorEngine::OnSignalDelegate()
{
    static FSignalDelegate onSignalDelegate;

    return onSignalDelegate;
}

void
FOdysseyVectorEngine::Signal( uint64 iSignalFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Signal);
    // Force invalidation when we need redrawing
    // This should be removed once we have per-rectangle invalidation
    if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW )
    {
        Invalidate();
    }

    OnSignalDelegate().Broadcast( mScene, iSignalFlags );
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
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4]) PIXELS;                                                 \
            double TEXUF = U * ( WIDTH  - 1 );                                                                             \
            double TEXVF = V * ( HEIGHT - 1 );                                                                             \
            int32  TEXUI = TEXUF;                                                                                          \
            int32  TEXVI = TEXVF;                                                                                          \
            double  WEIGHTU = TEXUF - TEXUI;                                                                               \
            double  WEIGHTV = TEXVF - TEXVI;                                                                               \
            double  INVWEIGHTU = 1.0f - WEIGHTU;                                                                           \
            double  INVWEIGHTV = 1.0f - WEIGHTV;                                                                           \
            uint32 OFFSETTOPLEFT     = ( TEXVI * WIDTH ) + TEXUI                                                           \
                 , OFFSETTOPRIGHT    = OFFSETTOPLEFT + 1                                                                   \
                 , OFFSETBOTTOMRIGHT = OFFSETTOPLEFT + 1 + WIDTH                                                           \
                 , OFFSETBOTTOMLEFT  = OFFSETTOPLEFT + WIDTH;                                                              \
            uint8 UPOL0, UPOL1;                                                                                            \
            uint8 VPOL0, VPOL1;                                                                                            \
                                                                                                                           \
            if( ( FLAGS & FPolygonDrawingFlags::BRUSHALPHAONLY ) == 0 )                                                    \
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
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4]) PIXELS; \
            int32 TEXU = U * ( WIDTH  - 1 );                               \
            int32 TEXV = V * ( HEIGHT - 1 );                               \
            uint32 TEXOFFSET = ( TEXV * WIDTH ) + TEXU;                    \
                                                                           \
            if( ( FLAGS & FPolygonDrawingFlags::BRUSHALPHAONLY ) == 0 )    \
            {                                                              \
                B = PIXELS32[TEXOFFSET][0];                                \
                G = PIXELS32[TEXOFFSET][1];                                \
                R = PIXELS32[TEXOFFSET][2];                                \
            }                                                              \
                                                                           \
            A = PIXELS32[TEXOFFSET][3];                                    \
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
    double opacityFactor = iOpacity / 255.0f;
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
                if( ( iPolygonDrawingFlags & FPolygonDrawingFlags::BILINEARFILTERING )
                 && (        x < (int32)(iImageWidth  - 1) )   // prevent overflow
                 && ( hline->y < (int32)(iImageHeight - 1) ) ) // prevent overflow
                {
                    GETPIXELBF( iBrushPixelData
                              , iBrushWidth
                              , iBrushHeight
                              , iBrushBitsPerPixel
                              , iPolygonDrawingFlags
                              , u >= 1.0f ? fmod(u,1.0f) : u// function call might slow things (maybe not that much, as fmod is declared inline)
                              , v >= 1.0f ? fmod(v,1.0f) : v// function call might slow things (maybe not that much, as fmod is declared inline)
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
                            , u >= 1.0f ? fmod(u,1.0f) : u// function call might slow things (maybe not that much, as fmod is declared inline)
                            , v >= 1.0f ? fmod(v,1.0f) : v// function call might slow things (maybe not that much, as fmod is declared inline)
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
                        double blending = (double) BA * opacityFactor;
                        double invBlending = 1.0f - blending;
                        uint32 maxAlpha = ( uint32) srcimg[aoffset][3] + ( BA * iOpacity );

                        srcimg[aoffset][0] = /*BB*/( invBlending * srcimg[aoffset][0] ) + ( BB * blending );
                        srcimg[aoffset][1] = /*BG*/( invBlending * srcimg[aoffset][1] ) + ( BG * blending );
                        srcimg[aoffset][2] = /*BR*/( invBlending * srcimg[aoffset][2] ) + ( BR * blending );
                        srcimg[aoffset][3] = ( maxAlpha > 255 ) ? 255 : maxAlpha;
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
FOdysseyVectorEngine::FillHexagon( BLContext* iBLContext
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
    const BLMatrix2D& userMatrix = iBLContext->userMatrix();
    BLPoint worldPoint[6] = { userMatrix.mapPoint( iPoint[0].x, iPoint[0].y )
                            , userMatrix.mapPoint( iPoint[1].x, iPoint[1].y )
                            , userMatrix.mapPoint( iPoint[2].x, iPoint[2].y )
                            , userMatrix.mapPoint( iPoint[3].x, iPoint[3].y )
                            , userMatrix.mapPoint( iPoint[4].x, iPoint[4].y )
                            , userMatrix.mapPoint( iPoint[5].x, iPoint[5].y ) };
    ::ULIS::FVec2I intPt[6] = { { (int32)worldPoint[0].x, (int32)worldPoint[0].y }
                              , { (int32)worldPoint[1].x, (int32)worldPoint[1].y }
                              , { (int32)worldPoint[2].x, (int32)worldPoint[2].y }
                              , { (int32)worldPoint[3].x, (int32)worldPoint[3].y }
                              , { (int32)worldPoint[4].x, (int32)worldPoint[4].y }
                              , { (int32)worldPoint[5].x, (int32)worldPoint[5].y } };

    int32 xmin = intPt[0].x;
    int32 xmax = intPt[0].x;
    int32 ymin = intPt[0].y;
    int32 ymax = intPt[0].y;

    for( int i = 1; i < 6; i++ )
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
                    , 6
                    , iOpacity
                    , iColor
                    , iBrushPixelData
                    , iBrushWidth
                    , iBrushHeight
                    , iBrushBitsPerPixel
                    , iPolygonDrawingFlags );
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
                 && ( mHorizontalLineBuffer[i].x0 < (int32) mRenderData.size.w ) )
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

void
FOdysseyVectorEngine::GetFocusedAncestorList( std::list<FOdysseyVectorObject*>& oObjectList )
{
    Traverse
    ( mScene
    , mScene
    , 0
    , [ this
      , &oObjectList ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( ObjectHasFocus( mScene, object, traversalFlags ) )
          {
              oObjectList.push_back( object );

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );
}

void
FOdysseyVectorEngine::GetFocusedObjectList( std::list<FOdysseyVectorObject*>& oObjectList )
{
    Traverse
    ( mScene
    , mScene
    , 0
    , [ this
      , &oObjectList ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( ObjectHasFocus( mScene, object, traversalFlags ) )
          {
              oObjectList.push_back( object );

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}


bool
FOdysseyVectorEngine::ObjectHasFocus( FOdysseyVectorGroupPaint* iScene
                                    , FOdysseyVectorObject* iObject
                                    , uint64 iTraversalFlags )
{
    if( iObject->IsSelected() )
    {
        return true;
    }

    if( GetSelectedObjectList().size() == 0 )
    {
        return true;
    }

    if( iTraversalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS )
    {
        return true;
    }

    return false;
}

// Execute callback on object tree
uint64
FOdysseyVectorEngine::Traverse( FOdysseyVectorGroupPaint* iScene
                              , FOdysseyVectorObject* iObject
                              , uint64 iTraversalFlags
                              , std::function<uint64(FOdysseyVectorObject*,uint64)> iCallback )
{
    uint64 objectTraversalFlags = iCallback( iObject, iTraversalFlags );

    if( objectTraversalFlags & TRAVERSE_STOP )
    {
        return TRAVERSE_STOP;
    }

    if( objectTraversalFlags & TRAVERSE_OBJECT_ACCEPTED )
    {
        iTraversalFlags |= TRAVERSE_PARENT_HASFOCUS;
    }

    if( ( objectTraversalFlags & TRAVERSE_OBJECT_IGNORE_CHILDREN ) == 0 )
    {
        for( FOdysseyVectorObject* childObject : iObject->GetChildrenList() )
        {
            uint64 childTraversalFlags = Traverse( iScene, childObject, iTraversalFlags, iCallback );

            if( childTraversalFlags & TRAVERSE_STOP )
            {
                return TRAVERSE_STOP;
            }
        }
    }

    return 0;
}

// static
FOdysseyVectorGroupPaint*
FOdysseyVectorEngine::MakePaintGroupFromObjects( FOdysseyVectorObject* iParent
                                              , const std::list<FOdysseyVectorObject*>& iObjectList
                                               , std::vector<FOdysseyVectorObject*>& oCubicPathArray
                                               , std::vector<FOdysseyVectorObject*>& oCubicPathOldParentArray
                                               , std::vector<FOdysseyVectorBucket*>& oRemovedBucketArray )
{
    // this array will help us to transfer buckets as well
    std::vector<FOdysseyVectorGroupPaint*> parentPaintGroupArray;

    if( iObjectList.size() )
    {
        FOdysseyVectorGroupPaint* paintGroup = new FOdysseyVectorGroupPaint( "Paint Group" );

        iParent->AppendChild( paintGroup );

        paintGroup->UpdateMatrix();

        for( FOdysseyVectorObject* selectedObject : iObjectList )
        {
            if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
            {
                FOdysseyVectorPath* selectedCubicPath = static_cast<FOdysseyVectorPath*>( selectedObject );

                oCubicPathArray.push_back( selectedCubicPath );
            }
        }

        oCubicPathOldParentArray.resize( oCubicPathArray.size() );

        for( int i = 0; i < oCubicPathArray.size(); i++ )
        {
            FOdysseyVectorObject* parentObject = oCubicPathArray[i]->GetParent();

            oCubicPathOldParentArray[i] = parentObject;

            paintGroup->TransferChild( oCubicPathArray[i], paintGroup->GetLastChild() );

            // take andvantge of this loop to also extract buckets if over
            if( parentObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* parentPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(parentObject);

                if( std::find( parentPaintGroupArray.begin(), parentPaintGroupArray.end(), parentPaintGroup ) == parentPaintGroupArray.end() )
                {
                    parentPaintGroupArray.push_back( parentPaintGroup );
                    // the gap tolerance will be set multiple times
                    // if there are multiples former parent paintgroups
                    // but this does not matter.
                    paintGroup->SetGapTolerance( parentPaintGroup->GetGapTolerance() );
                }
            }
        }

        // update paths and detect cycles for bucket matching
        paintGroup->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        // extract buckets if over
        for( FOdysseyVectorGroupPaint* parentPaintGroup : parentPaintGroupArray )
        {
            std::list<FOdysseyVectorBucket*>& bucketList = parentPaintGroup->GetBucketList();
            BLMatrix2D& parentWorldMatrix = parentPaintGroup->GetWorldMatrix();

            for( FOdysseyVectorBucket* bucket : bucketList )
            {
                ::ULIS::FVec2D& bucketCoords = bucket->GetCoords();
                BLPoint bucketWorldCoords = parentWorldMatrix.mapPoint( bucketCoords.x, bucketCoords.y );

                // TODO: that's a lot of conversion, kowing that both PickCycle 
                // and FOdysseyVectorBucket() convert to their own space.
                // We can optimize by creating a PickCycle
                // with local coordinates as parameter. Same for FOdysseyVectorBucket()
                if( paintGroup->PickCycle( bucketWorldCoords.x, bucketWorldCoords.y ) )
                {
                    FOdysseyVectorBucket* importedBucket = new FOdysseyVectorBucket( paintGroup, bucket );

                    paintGroup->AddBucket( importedBucket );

                    oRemovedBucketArray.push_back( bucket );
                }
            }
        }

        // we can't remove the bucket in the previous loop as it would alter the std:list
        // we are looping into (unless we copy the list, yes I know).
        for( int i = 0; i < oRemovedBucketArray.size(); i++ )
        {
            FOdysseyVectorObject* ownerObject = oRemovedBucketArray[i]->GetOwner();

            if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
            {
                FOdysseyVectorGroupPaint* parentPaintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);
            
                parentPaintGroup->RemoveBucket( oRemovedBucketArray[i] );
            }
        }

        return paintGroup;
    }

    return nullptr;
}

::ULIS::FVec2D
FOdysseyVectorEngine::GetPositionFromObjects( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    BLPoint averagePosition = BLPoint( 0.0f, 0.0f );

    if ( iObjectList.size() )
    {
        for( FOdysseyVectorObject* obj : iObjectList )
        {
            ::ULIS::FRectD bbox = obj->GetBBox( true );
            BLPoint middle = BLPoint( bbox.x + bbox.w * 0.5f
                                    , bbox.y + bbox.h * 0.5f );

            averagePosition.x += middle.x;
            averagePosition.y += middle.y;
        }

        averagePosition.x /= iObjectList.size();
        averagePosition.y /= iObjectList.size();
    }

    return ::ULIS::FVec2D( averagePosition.x, averagePosition.y );
}

// static
void
FOdysseyVectorEngine::FlipObjectsHorizontal( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    FlipObjects( iObjectList, -1.0f, 1.0f );
}

// static
void
FOdysseyVectorEngine::FlipObjectsVertical( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    FlipObjects( iObjectList, 1.0f, -1.0f );
}

// static
void
FOdysseyVectorEngine::FlipObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                                 , double iXFactor
                                 , double iYFactor )
{
    ::ULIS::FVec2D axisPosition = GetPositionFromObjects( iObjectList );
    BLMatrix2D inverseAxisMatrix;
    BLMatrix2D axisMatrix;
    BLMatrix2D flippingMatrix;

    axisMatrix.reset();
    axisMatrix.translate( axisPosition.x, axisPosition.y );

    BLMatrix2D::invert( inverseAxisMatrix, axisMatrix );

    flippingMatrix.resetToScaling( iXFactor, iYFactor );

    for( FOdysseyVectorObject *object : iObjectList )
    {
        BLMatrix2D& objectWorldMatrix = object->GetWorldMatrix();
        BLPoint objectWorldCenter = objectWorldMatrix.mapPoint( 0.0f, 0.0f );
        BLPoint objectLocalCenter = inverseAxisMatrix.mapPoint( objectWorldCenter ); 
        BLPoint objectLocalFlippedCenter = flippingMatrix.mapPoint( objectLocalCenter );
        BLPoint objectWorldFlippedCenter = axisMatrix.mapPoint( objectLocalFlippedCenter );

        objectLocalCenter = object->GetParent()->GetInverseWorldMatrix().mapPoint( objectWorldFlippedCenter );

        object->Translate( objectLocalCenter.x, objectLocalCenter.y );
        object->Scale( iXFactor * object->GetScalingX(), iYFactor * object->GetScalingY() );
        object->Rotate( -object->GetRotation() );

        object->UpdateMatrix();
    }
}

// static
FOdysseyVectorGroup*
FOdysseyVectorEngine::GroupObjects( FOdysseyVectorObject* iParent
                                  , const std::list<FOdysseyVectorObject*>& iObjectList
                                  , std::vector<FOdysseyVectorObject*>& oObjectArray
                                  , std::vector<FOdysseyVectorObject*>& oObjectOldParentArray )
{
    FOdysseyVectorGroupPaint* scene = iParent->GetScene();
    BLPoint averageTranslation = { 0.0f, 0.0f };

    if ( iObjectList.size() )
    {
        FOdysseyVectorGroup* group = new FOdysseyVectorGroup( FString("Group") );

        iParent->AppendChild ( group );

        //group->Translate( averageTranslation.x, averageTranslation.y );
        group->UpdateMatrix();

        oObjectArray.reserve( iObjectList.size() );
        oObjectOldParentArray.reserve( iObjectList.size() );

        for( FOdysseyVectorObject *obj : iObjectList )
        {
            if( obj != scene )
            {
                oObjectArray.push_back( obj );
                oObjectOldParentArray.push_back( obj->GetParent() );

                group->TransferChild( obj, group->GetLastChild() );
            }
        }

        group->Invalidate();

        return group;
    }

    return nullptr;
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
#endif // unused

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

void
FOdysseyVectorEngine::RemoveObjects( const std::list<FOdysseyVectorObject*>& iObjectList
                                   , std::vector<FOdysseyVectorObject*>& oRemovedObjectArray )
{
    for( FOdysseyVectorObject* vectorObject : iObjectList )
    {
        if( vectorObject->GetParent()->RemoveChild( vectorObject ) == FOdysseyVectorObject::HIERARCHY_CHANGE_SUCCESS )
        {
            oRemovedObjectArray.push_back( vectorObject );
        }
    }
}
