#pragma once

#include <blend2d.h>
#include <ULIS>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"

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

        virtual ~FOdysseyVectorHUD();
        FOdysseyVectorHUD();


        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) = 0;
        virtual void Reset( FOdysseyVectorScene* iScene ) = 0;

        void PickPoints( double iWorldX
                       , double iWorldY
                       , double iSelectionRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );

        void MakePointQuadTree( FOdysseyVectorScene *iScene );

    protected:
        FPointQuadTree* mPointQuadTree;
};
