#pragma once

#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorObject;
class FOdysseyVectorPoint;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorPath;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorScene;
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
                     , std::vector<FPointQuadTreeEntry>& iPointQuadTreeEntryArray );

        void Build( uint32 iMaxPointsPerQuad, std::vector<FPointQuadTreeEntry>& iParentPointQuadTreeEntryArray );
        void Draw( FOdysseyVectorScene* iScene, uint64 iFlags );
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
        static void DrawObjectRecursive( FOdysseyVectorObject* iObj, BLContext* iBLCtx );
        static FColor& GetForegroundColor();
        static FColor& GetBackgroundColor();
        static FColor& GetHighlightColor();

        static void DrawLine( double iWorldx0
                            , double iWorldY0
                            , double iWorldx1
                            , double iWorldY1
                            , BLContext* iBLContext
                            , const BLRgba32& fgColor
                            , const BLRgba32& bgColor );

        static void DrawCircle( double iWorldx
                              , double iWorldY
                              , double iRadius
                              , BLContext* iBLContext
                              , const BLRgba32& fgColor
                              , const BLRgba32& bgColor );

        static void DrawVertex( FOdysseyVectorVertex* iVertex
                              , BLContext* iBLContext
                              , const BLRgba32& fgColor
                              , const BLRgba32& bgColor
                              , const BLRgba32& hcColor
                              , bool iWorld
                              , uint64 iHUDFlags );

        static void DrawCubicSegment( FOdysseyVectorSegmentCubic* iCubicSegment
                                    , BLContext* iBLContext
                                    , const BLRgba32& fgColor
                                    , const BLRgba32& bgColor
                                    , const BLRgba32& hcColor
                                    , bool iWorld
                                    , uint64 iHUDFlags );

        static void DrawPath( FOdysseyVectorPath* iPath
                            , const BLRgba32& fgColor
                            , const BLRgba32& bgColor
                            , const BLRgba32& hcColor
                            , bool iWorld
                            , uint64 iHUDFlags );

        static void DrawPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup
                                  , const BLRgba32& fgColor
                                  , const BLRgba32& bgColor
                                  , const BLRgba32& hcColor
                                  , bool iWorld
                                  , uint64 iHUDFlags );

        virtual ~FOdysseyVectorHUD();
        FOdysseyVectorHUD();

        static const uint32 VERTEXRADIUS = 3;
        static const uint32 HANDLERADIUS = 3;

        // HUD Drawing Flags
        static const uint64 VIEW_VERTEX_VALENCE0 = 1 << 0;
        static const uint64 VIEW_VERTEX_VALENCE1 = 1 << 1;
        static const uint64 VIEW_VERTEX_VALENCE2 = 1 << 2;
        static const uint64 VIEW_VERTEX          = VIEW_VERTEX_VALENCE0
                                                 | VIEW_VERTEX_VALENCE1
                                                 | VIEW_VERTEX_VALENCE2;
        static const uint64 VIEW_VERTEX_HANDLE    = 1 << 3;
        static const uint64 VIEW_VERTEX_ALIGNMENT = 1 << 4;
        static const uint64 VIEW_SEGMENT_HANDLE   = 1 << 5;
        static const uint64 VIEW_SEGMENT          = 1 << 6;
        static const uint64 VIEW_ALL              = 0xFFFFFFFFFFFFFFFFULL;

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iHUDFlags ) = 0;
        virtual void Reset( FOdysseyVectorScene* iScene ) = 0;
        virtual void Load( FOdysseyVectorScene* iScene ) = 0;

        void PickPoints( double iWorldX
                       , double iWorldY
                       , double iSelectionRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );

        void MakePointQuadTree( FOdysseyVectorScene *iScene, bool iRestrictToSelection );


    protected:
        FPointQuadTree* mPointQuadTree;
};
