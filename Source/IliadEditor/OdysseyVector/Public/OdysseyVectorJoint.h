// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include "OdysseyVectorPolygon.h"

class FOdysseyVectorVertex;
class FOdysseyVectorSegment;
class FOdysseyVectorEngine;

class ODYSSEYVECTOR_API FOdysseyVectorJoint
{
    public:
        ~FOdysseyVectorJoint();
        FOdysseyVectorJoint( FOdysseyVectorVertex* iVertex );

        double
        GetLength();

        void
        Draw( BLContext* iBLContext
            , FOdysseyVectorEngine* iVectorEngine
            , double iStartU
            , double iEndU
            , double iCombinedOpacity
            , uint64 iDrawingFlags );

        std::vector<FOdysseyVectorPolygon3>& GetPolygonCache();
        void UpdateBBox();
        ::ULIS::FRectD GetBBox( bool iWorld );
        void Make( FOdysseyVectorSegment* iPrevSegment
                 , FOdysseyVectorSegment* iNextSegment );
        void ResetBBox();

        void   SetTextureU( double iTextureStartU, double iTextureEndU );
        double GetTextureStartU();
        double GetTextureEndU();


    private:
        void MakeNone();
        void MakeMiter( FOdysseyVectorSegment* iPrevSegment
                      , FOdysseyVectorSegment* iNextSegment
                      , const ::ULIS::FVec2D& iPrevEdgePoint
                      , const ::ULIS::FVec2D& iNextEdgePoint
                      , uint32 iSide );
        void MakeLinear( FOdysseyVectorSegment* iPrevSegment
                       , FOdysseyVectorSegment* iNextSegment
                       , const ::ULIS::FVec2D& iPrevEdgePoint
                       , const ::ULIS::FVec2D& iNextEdgePoint
                       , uint32 iSide );
        void MakeRadial( FOdysseyVectorSegment* iPrevSegment
                       , FOdysseyVectorSegment* iNextSegment
                       , const ::ULIS::FVec2D& iPrevEdgePoint
                       , const ::ULIS::FVec2D& iNextEdgePoint
                       , uint32 iSide );
        double GetEdgePoints( FOdysseyVectorSegment* iPrevSegment
                            , FOdysseyVectorSegment* iNextSegment
                            , ::ULIS::FVec2D iPrevEdgePoint[2]
                            , ::ULIS::FVec2D iNextEdgePoint[2] );

    protected:
        std::vector<FOdysseyVectorPolygon3> mPolygonCache;
        ::ULIS::FVec2D mPrevEdgePoint[2];
        ::ULIS::FVec2D mNextEdgePoint[2];
        FOdysseyVectorVertex* mVertex;
        ::ULIS::FRectD mBBox;
        double mLength;
        double mTextureStartU;
        double mTextureEndU;
};
