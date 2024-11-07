#pragma once

#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorBucket;
class FOdysseyVectorObject;
class FOdysseyVectorVertex;
class FOdysseyVectorPoint;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;

typedef struct _FPointQuadTreeEntry
{
    FOdysseyVectorPoint* point;
    ::ULIS::FVec2D worldCoords;

    _FPointQuadTreeEntry( FOdysseyVectorPoint* iPoint, ::ULIS::FVec2D iWorldCoords )
    {
        point = iPoint;
        worldCoords = iWorldCoords;
    }
} FPointQuadTreeEntry;

class ODYSSEYVECTOR_API FPointQuadTree
{
    public:
       ~FPointQuadTree();
       FPointQuadTree( const ::ULIS::FRectD& iRect
                     , uint32 iMaxPointsPerQuad
                     , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray
                     , uint32 iDepth
                     , uint32 iMaxDepth );

        void Build( uint32 iMaxPointsPerQuad
                     , std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray
                     , uint32 iDepth
                     , uint32 iMaxDepth );
        void Draw( BLContext* iBLContext
                 , FOdysseyVectorGroupPaint* iScene
                 , uint64 iFlags );
        void PickPoints( double iWorldX
                       , double iWorldY
                       , double iSelectionRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );
    private:
        std::vector<FPointQuadTreeEntry> mPointQuadTreeEntryArray;
        FPointQuadTree* mChildren[4];
        ::ULIS::FRectD mRect;
};

class ODYSSEYVECTOR_API FOdysseyVectorHUD
{
    public:
        static bool IsPaintedPath( FOdysseyVectorObject* iObject, bool iHasParentSelected );

        static FColor& GetForegroundColor();
        static FColor& GetBackgroundColor();
        static FColor& GetHighlightColor();
        static void DrawLine( BLContext* iBLContext
                            , double iWorldx0
                            , double iWorldY0
                            , double iWorldx1
                            , double iWorldY1
                            , const BLRgba32& fgColor
                            , const BLRgba32& bgColor );

        static void DrawCenteredSquare( BLContext* iBLContext
                                      , double iWorldx
                                      , double iWorldY
                                      , double iRadius
                                      , const BLRgba32& fgColor
                                      , const BLRgba32& bgColor );

        static void DrawCircle( BLContext* iBLContext
                              , double iWorldx
                              , double iWorldY
                              , double iRadius
                              , const BLRgba32& fgColor
                              , const BLRgba32& bgColor );

        static void DrawVertex( BLContext* iBLContext
                              , FOdysseyVectorVertex* iVertex
                              , const BLRgba32& fgColor
                              , const BLRgba32& bgColor
                              , const BLRgba32& hcColor
                              , bool iWorld
                              , uint64 iHUDFlags );

        static void DrawCubicSegment( BLContext* iBLContext
                                    , FOdysseyVectorSegmentCubic* iCubicSegment
                                    , const BLRgba32& fgColor
                                    , const BLRgba32& bgColor
                                    , const BLRgba32& hcColor
                                    , bool iWorld
                                   , uint64 iHUDFlags );

        static void DrawPath( BLContext* iBLContext
                            , FOdysseyVectorPath* iPath
                            , const BLRgba32& fgColor
                            , const BLRgba32& bgColor
                            , const BLRgba32& hcColor
                            , bool iWorld
                            , uint64 iHUDFlags );

        static ::ULIS::FVec2D GetBucketPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        static ::ULIS::FVec2D GetBucketRadialHandlePosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        static ::ULIS::FVec2D GetBucketRadialPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        static ::ULIS::FVec2D GetBucketHandleVector( FOdysseyVectorBucket* iBucket, bool iWorld );

        static void DrawBucket( BLContext* iBLContext
                              , FOdysseyVectorBucket* iBucket
                              , const BLRgba32& fgColor
                              , const BLRgba32& bgColor
                              , const BLRgba32& hcColor
                              , uint64 iHUDFlags );

        static void DrawGroupPaint( BLContext* iBLContext
                                  , FOdysseyVectorGroupPaint* iPaintGroup
                                  , const BLRgba32& fgColor
                                  , const BLRgba32& bgColor
                                  , const BLRgba32& hcColor
                                  , bool iWorld
                                  , uint64 iHUDFlags );

        virtual ~FOdysseyVectorHUD();
        FOdysseyVectorHUD();

        static constexpr  double VERTEXRADIUS_SMALL = 2.0f;
        static constexpr  double HANDLERADIUS_SMALL = 1.5;

        static constexpr  double VERTEXRADIUS = 3.0f;
        static constexpr  double HANDLERADIUS = 2.5f;

        // Bucket constants
        static constexpr  double HANDLE_RADIUS      = 3;
        static constexpr  double HANDLE_DISTANCE    = 40;
        static constexpr  double PELLET_RADIUS      = 5;
        static constexpr  double RADIAL_AREA_RADIUS = 40;

        static const uint32 PICK_NONE          = 0;
        static const uint32 PICK_HANDLE        = 1;
        static const uint32 PICK_BUCKET        = 2;
        static const uint32 PICK_RADIAL_HANDLE = 3;
        static const uint32 PICK_RADIAL_AREA   = 4;
        static const uint32 PICK_PROPAGATE     = 5;

        // HUD Drawing Flags
        static const uint64 HUD_MODE_OBJECT              = ( 1ULL <<  0 );
        static const uint64 HUD_MODE_VERTEX              = ( 1ULL <<  1 );
        static const uint64 HUD_MODE_ALL                 = HUD_MODE_OBJECT
                                                         | HUD_MODE_VERTEX;
        static const uint64 HUD_PATH_VERTEX_VALENCE0     = ( 1ULL <<  2 );
        static const uint64 HUD_PATH_VERTEX_VALENCE1     = ( 1ULL <<  3 );
        static const uint64 HUD_PATH_VERTEX_VALENCE2     = ( 1ULL <<  4 );
        static const uint64 HUD_PATH_VERTEX              = HUD_PATH_VERTEX_VALENCE0
                                                         | HUD_PATH_VERTEX_VALENCE1
                                                         | HUD_PATH_VERTEX_VALENCE2;
        static const uint64 HUD_PATH_VERTEX_HANDLE       = ( 1ULL <<  5 );
        static const uint64 HUD_PATH_VERTEX_ALIGNMENT    = ( 1ULL <<  6 );
        static const uint64 HUD_PATH_SEGMENT             = ( 1ULL <<  7 );
        static const uint64 HUD_PATH_SEGMENT_HANDLE      = ( 1ULL <<  8 );
        static const uint64 HUD_PATH_ALL                 = HUD_PATH_VERTEX
                                                         | HUD_PATH_VERTEX_HANDLE
                                                         | HUD_PATH_SEGMENT
                                                         | HUD_PATH_SEGMENT_HANDLE;
        static const uint64 HUD_GROUPPAINT_BUCKET        = ( 1ULL <<  9 );
        static const uint64 HUD_GROUPPAINT_BUCKET_HANDLE = ( 1ULL << 10 );
        static const uint64 HUD_GROUPPAINT_ALL           = HUD_GROUPPAINT_BUCKET
                                                         | HUD_GROUPPAINT_BUCKET_HANDLE;
        static const uint64 HUD_SELECTIONBOX             = ( 1ULL << 11 );
        static const uint64 HUD_SIZE_SMALL               = ( 1ULL << 12 );
        static const uint64 HUD_DRAW_ALL                 = ( 1ULL << 13 );
        //static const uint64 VIEW_ALL              = 0xFFFFFFFFFFFFFFFFULL;

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) = 0;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) = 0;


        void PickPoints( double iWorldX
                       , double iWorldY
                       , double iSelectionRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );

        void MakePointQuadTree( FOdysseyVectorGroupPaint *iScene
                              , bool iFocusedObjectsOnly
                              , uint64 iHUDFlags );

    protected:
        FPointQuadTree* mPointQuadTree;
};
