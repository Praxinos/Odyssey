// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>

#include "InbetweenerTag/InbetweenerChart.h"
#include "InbetweenerTag/InbetweenerGrid.h"

class FOdysseyVectorBucket;
class FOdysseyVectorObject;
class FOdysseyVectorVertex;
class FOdysseyVectorPoint;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;
class FOdysseyVectorTagInbetweener;
class FInbetweenerBreakdown;
class FInterpolatedPath;


class ODYSSEYVECTOR_API IOdysseyVectorHUD
{
    public:
        virtual void Reset() = 0;
        virtual void Draw( BLContext* iBLContext ) = 0;
};

class ODYSSEYVECTOR_API FOdysseyVectorHUD
{
    public:
        static bool IsPaintedPath( FOdysseyVectorObject* iObject, bool iHasParentSelected );

        static FColor& GetForegroundColor();
        static FColor& GetBackgroundColor();
        static FColor& GetHighlightColor();

        static void DrawCenteredSquare( BLContext* iBLContext
                                      , double iWorldx
                                      , double iWorldY
                                      , double iRadius
                                      , const BLRgba32& fgColor
                                      , const BLRgba32& bgColor );

        static ::ULIS::FVec2D GetBucketPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        static ::ULIS::FVec2D GetBucketRadialHandlePosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        static ::ULIS::FVec2D GetBucketRadialPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        static ::ULIS::FVec2D GetBucketHandleVector( FOdysseyVectorBucket* iBucket, bool iWorld );

        static constexpr  double VERTEXRADIUS = 4.0f;
        static constexpr  double HANDLERADIUS = 3.0f;

        // Bucket constants
        static constexpr  double HANDLE_RADIUS      = 4;
        static constexpr  double HANDLE_DISTANCE    = 40;
        static constexpr  double PELLET_RADIUS      = 8;
        static constexpr  double RADIAL_AREA_RADIUS = 20;

        // HUD Drawing Flags
        static const uint64 HUD_MODE_OBJECT              = ( 1ULL <<  0 );
        static const uint64 HUD_MODE_VERTEX              = ( 1ULL <<  1 );
        static const uint64 HUD_MODE_INBETWEEN           = ( 1ULL <<  2 );
        static const uint64 HUD_MODE_OBJECT_ALLOWED      = ( 1ULL <<  3 );
        static const uint64 HUD_MODE_VERTEX_ALLOWED      = ( 1ULL <<  4 );
        static const uint64 HUD_MODE_INBETWEEN_ALLOWED   = ( 1ULL <<  5 );
        static const uint64 HUD_MODE_ALL                 = HUD_MODE_OBJECT
                                                         | HUD_MODE_VERTEX
                                                         | HUD_MODE_INBETWEEN;
        static const uint64 HUD_PATH_VERTEX_VALENCE0     = ( 1ULL <<  6 );
        static const uint64 HUD_PATH_VERTEX_VALENCE1     = ( 1ULL <<  7 );
        static const uint64 HUD_PATH_VERTEX_VALENCE2     = ( 1ULL <<  8 );
        static const uint64 HUD_PATH_VERTEX              = HUD_PATH_VERTEX_VALENCE0
                                                         | HUD_PATH_VERTEX_VALENCE1
                                                         | HUD_PATH_VERTEX_VALENCE2;
        static const uint64 HUD_PATH_VERTEX_HANDLE       = ( 1ULL <<  9 );
        static const uint64 HUD_PATH_VERTEX_ALIGNMENT    = ( 1ULL << 10 );
        static const uint64 HUD_PATH_SEGMENT             = ( 1ULL << 11 );
        static const uint64 HUD_PATH_SEGMENT_HANDLE      = ( 1ULL << 12 );
        static const uint64 HUD_PATH_ALL                 = HUD_PATH_VERTEX
                                                         | HUD_PATH_VERTEX_HANDLE
                                                         | HUD_PATH_SEGMENT
                                                         | HUD_PATH_SEGMENT_HANDLE;
        static const uint64 HUD_GROUPPAINT_BUCKET        = ( 1ULL << 13 );
        static const uint64 HUD_GROUPPAINT_BUCKET_HANDLE = ( 1ULL << 14 );
        static const uint64 HUD_GROUPPAINT_ALL           = HUD_GROUPPAINT_BUCKET
                                                         | HUD_GROUPPAINT_BUCKET_HANDLE;
        static const uint64 HUD_BREAKDOWN_SOURCE         = ( 1ULL << 15 );
        static const uint64 HUD_BREAKDOWN_TARGET         = ( 1ULL << 16 );
        static const uint64 HUD_BREAKDOWN_INBETWEEN      = ( 1ULL << 17 );
        static const uint64 HUD_INBETWEEN_FADEFROMTARGET = ( 1ULL << 18 );
        static const uint64 HUD_INBETWEEN_FADEFROMSOURCE = ( 1ULL << 19 );
        static const uint64 HUD_INBETWEEN_FADERELATIVE   = ( 1ULL << 20 );
        static const uint64 HUD_BREAKDOWN_SOURCE_GRID    = ( 1ULL << 21 );
        static const uint64 HUD_BREAKDOWN_TARGET_GRID    = ( 1ULL << 22 );
        static const uint64 HUD_BREAKDOWN_GRID_DOTTED    = ( 1ULL << 23 );
        static const uint64 HUD_TAGINBETWEENER_ALL       = HUD_BREAKDOWN_SOURCE
                                                         | HUD_BREAKDOWN_TARGET
                                                         | HUD_BREAKDOWN_SOURCE_GRID
                                                         | HUD_BREAKDOWN_TARGET_GRID
                                                         | HUD_BREAKDOWN_INBETWEEN;
        static const uint64 HUD_SELECTIONBOX             = ( 1ULL << 24 );
        static const uint64 HUD_DRAW_ALL                 = ( 1ULL << 26 );
        //static const uint64 VIEW_ALL              = 0xFFFFFFFFFFFFFFFFULL;
};
