#include "OdysseyVectorEngine.h"
//#include <future>

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
        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorEngine::UnselectObject( FOdysseyVectorObject* iVecObj )
{
    iVecObj->SetIsSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
FOdysseyVectorEngine::SelectObject( FOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetIsSelected( true );

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

    //UseImage( iBLImage );

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

    //UseImage( &mDefaultBLImage );
}
/*
// static
void
FOdysseyVectorEngine::GetVertexSelection( std::list<FOdysseyVectorObject*>& iVectorObjectList
                                        , std::vector<FOdysseyVectorPoint*>& iSelectedPointArray )
{
    for( FOdysseyVectorObject* vectorObject : iVectorObjectList )
    {
        GetVertexSelectionRecursive( vectorObject, iSelectedPointArray );
    }
}

// static
void
FOdysseyVectorEngine::GetVertexSelectionRecursive( FOdysseyVectorObject* iObject
                                                 , std::vector<FOdysseyVectorPoint*>& iSelectedPointArray )
{
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        path->GetSelectedPoints( iSelectedPointArray, ePointSelectionFlags::Vertex );
    }

    for( FOdysseyVectorObject* childObject : iObject->GetChildrenList() )
    {
        GetVertexSelectionRecursive( childObject, iSelectedPointArray );
    }
}
*/
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
/*
uint64
FOdysseyVectorEngine::GetDrawingFlags()
{
    return mDrawingFlags;
}

void
FOdysseyVectorEngine::SetDrawingFlags( uint64 iDrawingFlags )
{
    mDrawingFlags = iDrawingFlags;
}
*/
void
FOdysseyVectorEngine::Render( BLContext* iBLContext, uint64 iDrawingFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Render);
    BLImage* image = iBLContext->targetImage();
    BLImageData imageData;

    image->getData( &imageData );

    // for drawing polygones (textured)
    if( mHorizontalLineBuffer.size() != imageData.size.h )
    {
        mHorizontalLineBuffer.resize( imageData.size.h );
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

    mInvalidatedRect = ::ULIS::FRectI( 0, 0, imageData.size.w, imageData.size.h );
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

// Macro for faster execution. Indeed, an inline function is not guaranteed to be inlined.
#define GETPIXEL(PIXELS,WIDTH,HEIGHT,BITSPERPIXEL,ALPHAONLY,U,V,R,G,B,A)   \
    switch ( BITSPERPIXEL )                                                \
    {                                                                      \
        case 32 :                                                          \
        {                                                                  \
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4]) PIXELS; \
            int32 TEXU = U * ( WIDTH  - 1 );                               \
            int32 TEXV = V * ( HEIGHT - 1 );                               \
            uint32 TEXOFFSET = ( TEXV * WIDTH ) + TEXU;                    \
                                                                           \
            if( ALPHAONLY == false )                                       \
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

void
FOdysseyVectorEngine::TraceHorizontalLine ( int32  iLineNumber
                                          , double iOpacity
                                          , int8*  iImagePixelData
                                          , uint32 iImageWidth
                                          , uint32 iImageHeight
                                          , int32  iImageBitsPerPixel
                                          , const  FColor& iColor
                                          // Temp
                                          , int8*  iBrushPixelData
                                          , uint32 iBrushWidth
                                          , uint32 iBrushHeight
                                          , int32  iBrushBitsPerPixel
                                          , bool   iBrushAlphaOnly )
{
    FHorizontalLine *hline = &mHorizontalLineBuffer[iLineNumber];
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
    uint32 offset = ( iLineNumber * iImageWidth );
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

    // Commented out: we don't drow from edge-to-edge, we stop 1 pixel before to prevent overlapping,
    // which would lead to double stroke and would produce artefact when alpha is semi-transparent.
    //for( int i = 0; i <= ddx; i++ )
    for( int i = 0; ( i < screenx ) && ( x < (int)iImageWidth /* clipping */ ); i++ )
    {
        if( ( x >= 0 ) && ( x < (int32) iImageWidth ) )
        {
            uint32 aoffset = offset + x;

            if( iBrushPixelData && iBrushWidth && iBrushHeight )
            {
                GETPIXEL( iBrushPixelData
                        , iBrushWidth
                        , iBrushHeight
                        , iBrushBitsPerPixel
                        , iBrushAlphaOnly
                        , fmod(u,1.0f) // function call might slow things (maybe not that much, as fmod is declared inline)
                        , fmod(v,1.0f) // function call might slow things (maybe not that much, as fmod is declared inline)
                        , BR
                        , BG
                        , BB
                        , BA );
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
/*
void
FOdysseyVectorEngine::DrawQuadThread( uint32 iProcessorID
                                    , uint32 iProcessorCount
                                    , int32  iFirstLine
                                    , int32  iLastLine
                                    , double iOpacity
                                    , int8*  iPixelData
                                    , int32  iBitsPerPixel
                                    // Temp
                                    , int8*  iBrushPixelData
                                    , uint32 iBrushWidth
                                    , uint32 iBrushHeight
                                    , int32  iBrushBitsPerPixel )
{
    for( int i = iFirstLine + iProcessorID; i <= iLastLine ; i += iProcessorCount )
    {
        if( mHorizontalLineBuffer[i].inited == 2 )
        {
            TraceHorizontalLine( i
                               , iOpacity
                               , iPixelData
                               , iBitsPerPixel
                               , iBrushPixelData
                               , iBrushWidth
                               , iBrushHeight
                               , iBrushBitsPerPixel );
        }

        mHorizontalLineBuffer[i].inited = 0;
    }
}
*/

void
FOdysseyVectorEngine::DrawPolygon( ::ULIS::FVec2I* iPoint
                                 , double* iU
                                 , double* iV
                                 , uint32 pointCount
                                 , double iOpacity
                                 , int8*  iImagePixelData
                                 , uint32 iImageWidth
                                 , uint32 iImageHeight
                                 , int32  iImageBitsPerPixel
                                 , const FColor& iColor
                                 // temp
                                 , int8*  iBrushPixelData
                                 , uint32 iBrushWidth
                                 , uint32 iBrushHeight
                                 , int32  iBrushBitsPerPixel
                                 , bool   iBrushAlphaOnly )
{
    int32 ymin = iPoint[0].y,
          ymax = ymin;

    for( uint32 i = 0; i < pointCount; i++ )
    {
        uint32 n = ( i + 1 ) % pointCount;

        if ( iPoint[i].y < ymin ) ymin = iPoint[i].y;
        if ( iPoint[i].y > ymax ) ymax = iPoint[i].y;

        // always draw in the same direction (left to right ) to avoid bad overlapping
        if( iPoint[i].x < iPoint[n].x )
        {
            TraceLine ( iPoint[i].x, iPoint[i].y, iU[i], iV[i]
                      , iPoint[n].x, iPoint[n].y, iU[n], iV[n], iImageWidth, iImageHeight );
        }
        else
        {
            TraceLine ( iPoint[n].x, iPoint[n].y, iU[n], iV[n]
                      , iPoint[i].x, iPoint[i].y, iU[i], iV[i], iImageWidth, iImageHeight );
        }
    }

    if ( ymin <  0                    ) ymin = 0;
    if ( ymin >= (int32) iImageHeight ) ymin = (int32) iImageHeight - 1;
    if ( ymax <  0                    ) ymax = 0;
    if ( ymax >= (int32) iImageHeight ) ymax = (int32) iImageHeight - 1;

    if ( ymin <= ymax )
    {
/*
        FOdysseyVectorComputer& mainComputer = FOdysseyVectorComputer::GetMainComputer();

        mainComputer.Run( [ this
                          , &ymin
                          , &ymax
                          , &iOpacity
                          , &iImagePixelData
                          , &iImageWidth
                          , &iImageHeight
                          , &iImageBitsPerPixel
                          , &iColor
                          , &iBrushPixelData
                          , &iBrushWidth
                          , &iBrushHeight
                          , &iBrushBitsPerPixel
                          , &iBrushAlphaOnly ]( uint32 iProcessorID, uint32 iProcessorCount ) -> bool
                          {
                              for( int i = ymin + iProcessorID; i <= ymax ; i += iProcessorCount )
                              {
                                  if( mHorizontalLineBuffer[i].inited == 2 )
                                  {
                                      TraceHorizontalLine( i
                                                         , iOpacity
                                                         , iImagePixelData
                                                         , iImageWidth
                                                         , iImageHeight
                                                         , iImageBitsPerPixel
                                                         , iColor
                                                         , iBrushPixelData
                                                         , iBrushWidth
                                                         , iBrushHeight
                                                         , iBrushBitsPerPixel
                                                         , iBrushAlphaOnly );
                                  }

                                  mHorizontalLineBuffer[i].inited = 0;

                                  if( i == ymax )
                                  {
                                      return true;
                                  }
                              }

                              return false;
                          } );
*/
/*
        std::vector<std::future<void>> threads;

//mProcessorCount = 2;

        threads.resize( mProcessorCount );

        int totalThreads = ( ymax - ymin  + 1 ) < (int) mProcessorCount ? ( ymax - ymin  + 1 ) :  (int)mProcessorCount;

        for( int32 i = 0; i < totalThreads; i++ )
        //for( uint32 i = 0; i < threads.size(); i++ )
        {
            threads[i] = std::async( std::launch::async
                                    , [ this
                                      , &ymin
                                      , &ymax
                                      , &iOpacity
                                      , &iPixelData
                                      , &iBitsPerPixel
                                      , &iBrushPixelData
                                      , &iBrushWidth
                                      , &iBrushHeight
                                      , &iBrushBitsPerPixel]( uint32 iProcessorID, uint32 iProcessorCount )
                                      {

                                          FGenericPlatformProcess::SetThreadAffinityMask( (uint64) 1 << iProcessorID );

                                          for( int i = ymin + iProcessorID; i <= ymax ; i += iProcessorCount )
                                          {
                                              if( mHorizontalLineBuffer[i].inited == 2 )
                                              {
                                                    TraceHorizontalLine( i
                                                                     , iOpacity
                                                                     , iPixelData
                                                                     , iBitsPerPixel
                                                                     , iBrushPixelData
                                                                     , iBrushWidth
                                                                     , iBrushHeight
                                                                     , iBrushBitsPerPixel );
                                              }

                                              mHorizontalLineBuffer[i].inited = 0;
                                          }
                                      }
                                  , i
                                  , mProcessorCount );
        }

        for( uint32 i = 0; i < (uint32)totalThreads; i++ )
        {
            threads[i].wait();
        }
*/
/*
        int32 lineCount = ( ymax - ymin ) + 1;

        ParallelFor( lineCount
                  , [ this
                    , &ymin
                    , &iOpacity
                    , iPixelData
                    , &iBitsPerPixel
                    , iBrushPixelData
                    , &iBrushWidth
                    , &iBrushHeight
                    , &iBrushBitsPerPixel ]( int32 iIndex )
                      {
                          int32 lineID = iIndex + ymin;

                          if( mHorizontalLineBuffer[lineID].inited == 2 )
                          {
                              TraceHorizontalLine( lineID
                                                 , iOpacity
                                                 , iPixelData
                                                 , iBitsPerPixel
                                                 , iBrushPixelData
                                                 , iBrushWidth
                                                 , iBrushHeight
                                                 , iBrushBitsPerPixel );
                          }

                          mHorizontalLineBuffer[lineID].inited = 0;
                      } );
*/

    // Single CPU version. The one that actually works.

        for ( int i = ymin; i <= ymax; i++ )
        {
            if( mHorizontalLineBuffer[i].inited == 2 )
            {
                if( mHorizontalLineBuffer[i].x1 >= 0 )
                {
                    TraceHorizontalLine( i
                                       , iOpacity
                                       , iImagePixelData
                                       , iImageWidth
                                       , iImageHeight
                                       , iImageBitsPerPixel
                                       , iColor
                                       , iBrushPixelData
                                       , iBrushWidth
                                       , iBrushHeight
                                       , iBrushBitsPerPixel
                                       , iBrushAlphaOnly );
                }
            }

            mHorizontalLineBuffer[i].inited = 0;
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

    if( iTraversalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_ACCEPTED )
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
        iTraversalFlags |= TRAVERSE_PARENT_ACCEPTED;
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
            oObjectArray.push_back( obj );
            oObjectOldParentArray.push_back( obj->GetParent() );

            group->TransferChild( obj, group->GetLastChild() );
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
