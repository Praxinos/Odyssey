// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include "OdysseyVectorPolygon.h"

class FOdysseyVectorPath;

class ODYSSEYVECTOR_API FOdysseyVectorJoint
{
    public:
        ~FOdysseyVectorJoint();
        FOdysseyVectorJoint( FOdysseyVectorPath* iPath );
        void SetPath( FOdysseyVectorPath* iPath );

        double
        GetLength();

        void
        Draw( BLContext* iBLContext
            , double iStartU
            , double iEndU
            , double iCombinedOpacity
            , uint64 iDrawingFlags );

        void MakeNone();

        void MakeMiter( ::ULIS::FVec2D& iOrigin
                      , ::ULIS::FVec2D& iVector0
                      , ::ULIS::FVec2D& iVector1
                      , double iRadius
                      , double iMiterLimit );

        void MakeLinear( ::ULIS::FVec2D& iOrigin
                       , ::ULIS::FVec2D& iVector0
                       , ::ULIS::FVec2D& iVector1
                       , double iRadius );

        void MakeRadial( ::ULIS::FVec2D& iOrigin
                       , ::ULIS::FVec2D& iVector0
                       , ::ULIS::FVec2D& iVector1
                       , double iRadius );

        std::vector<FOdysseyVectorPolygon3>& GetPolygonCache();

    protected:
        std::vector<FOdysseyVectorPolygon3> mPolygonCache;
        FOdysseyVectorPath* mPath;
        double mLength;
};
