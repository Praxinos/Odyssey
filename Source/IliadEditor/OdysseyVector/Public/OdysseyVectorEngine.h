// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorObject.h"

#include "OdysseyInvalidTileMap.h"


//#include "OdysseyVectorGroupPaint.generated.h"

// I use namespaces and not ENUM_CLASS_FLAGS because I can't use them as bool without a cast, which is idiotic.
namespace FPolygonDrawingFlags
{
    static const uint64 NONE              = 0;
    static const uint64 STROKE0           = 1ULL << 0;
    static const uint64 STROKE1           = 1ULL << 1;
    static const uint64 STROKE2           = 1ULL << 2;
    static const uint64 STROKE3           = 1ULL << 3;
    static const uint64 STROKEALL         = STROKE0 | STROKE1 | STROKE2 | STROKE3;
    static const uint64 FILL              = 1ULL << 4;
    static const uint64 NOOVERLAP         = 1ULL << 5;
    static const uint64 BRUSHALPHAONLY    = 1ULL << 6;
    static const uint64 BILINEARFILTERING = 1ULL << 7;
};

typedef struct _FHorizontalLine
{
    int32 y;
    int32 x0;
    double u0;
    double v0;
    int32 x1;
    double u1;
    double v1;
    int inited;

    _FHorizontalLine()
    {
        inited = 0;
    }
}
FHorizontalLine;

class ODYSSEYVECTOR_API FOdysseyVectorEngine
{
    public:
        DECLARE_MULTICAST_DELEGATE_TwoParams( FNotifyDelegate, FOdysseyVectorGroupPaint*, uint64 iDelegateFlags )

    public:
        // drawing flags
        static const uint64 DRAWING_WIREFRAME         = ( 1ULL <<  2 );
        static const uint64 DRAWING_IGNORECOLOR       = ( 1ULL <<  3 );


        // Notifications flags
        static const uint64 NOTIFY_ALL                = 0x0FFFFFFFFFFFFFFF;
        static const uint64 NOTIFY_RESERVED_SHIFT     = ( 15 );

        static FNotifyDelegate& OnNotifyDelegate();
        //       FInvalidateDelegate& OnInvalidateDelegate();

        /**
         * @brief Destructor
         */
        ~FOdysseyVectorEngine();

        /**
         * @brief Constructor
         */
        FOdysseyVectorEngine();

        /**
         * @brief Render the scene to the current buffer
         */
        //void Render();

        /**
         * @brief Returns the invalid Tile map containing invalid rects
         *
         * @return const FOdysseyInvalidTileMap&
         */
        FOdysseyInvalidTileMap& GetInvalidTileMap();

        /**
         * @brief render the current HUD.
         */
        void RenderHUD( BLContext* iBLContext
                      , FOdysseyVectorGroupPaint* iScene );

        /**
         * @brief Send a signal to methods registered to this delegate.
         * @param iSignalFlags SIGNAL_* flags that can be interpreted by the receiver of the signal.
         */
        static void Notify( FOdysseyVectorGroupPaint* iScene, uint64 iNotifyFlags );

        ::ULIS::FRectD Render( BLContext* iBLContext
                             , const ::ULIS::FRectD& iRedrawRect
                             , FOdysseyVectorGroupPaint* iScene
                             , uint64 iDrawingFlags );

        void TraceLine ( int32 iX0
                       , int32 iY0
                       , double iU0
                       , double iV0
                       , int32 iX1
                       , int32 iY1
                       , double iU1
                       , double iV1
                       , uint32 iImageWidth
                       , uint32 iImageHeight );

        void FillQuad( BLContext* iBLContext
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
                     , uint64 iPolygonDrawingFlags );

        void FillTriangle( BLContext* iBLContext
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
                         , uint64 iPolygonDrawingFlags );

        BLImageData& GetRenderData();

        void DrawLineAA( int32 x0
                       , int32 y0
                       , int32 x1
                       , int32 y1
                       , const int8* iImagePixelData
                       , uint32 iImageWidth
                       , uint32 iImageHeight
                       , int32  iImageBitsPerPixel
                       , const FColor& iColor );

        uint64 GetInvalidationFlags();
        ::ULIS::FRectD SanitizeRect( const ::ULIS::FRectD& iRenderRect
                                   , double iScreenWidth
                                   , double iScreenHeight );

    protected:
        void TracePolygon( const  ::ULIS::FVec2I* iPoint
                         , const  double* iU
                         , const  double* iV
                         , uint32 pointCount
                         , double iOpacity
                         , const  FColor& iColor
                         // temp
                         , const  int8* iBrushPixelData
                         , uint32 iBrushWidth
                         , uint32 iBrushHeight
                         , int32  iBrushBitsPerPixel
                         , uint64 iPolygonDrawingFlags );

    protected:
        uint64 mInvalidationFlags;
        //FInvalidateDelegate mOnInvalidateDelegate;
        //FOdysseyInvalidTileMap mInvalidTileMap;
        std::vector<FHorizontalLine> mHorizontalLineBuffer;
        uint32 mProcessorCount;
        BLImageData mRenderData; // for direct drawing via our own drawing routines.
        //uint64 mDrawingFlags; // temporary, until we find a way to pass the drawing flags as arg
        // mutex to prevent drawing whil update isn't complete. this is necessary due to the Proxy renderer
        // which runs in a different thread
        std::mutex mDrawingMutex;
};
