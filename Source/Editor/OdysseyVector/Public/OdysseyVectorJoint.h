// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include "OdysseyVectorPolygon.h"

class FOdysseyVectorVertex;
class FOdysseyVectorSegment;

class ODYSSEYVECTOR_API FOdysseyVectorJoint
{
    public:
        ~FOdysseyVectorJoint();
        FOdysseyVectorJoint( FOdysseyVectorVertex* iVertex );

        double
        GetLength();

        void
        Draw( BLContext* iBLContext
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

    private:
        void MakeNone();
        void MakeMiter( FOdysseyVectorSegment* iPrevSegment
                      , FOdysseyVectorSegment* iNextSegment );
        void MakeLinear( FOdysseyVectorSegment* iPrevSegment
                       , FOdysseyVectorSegment* iNextSegment );
        void MakeRadial( FOdysseyVectorSegment* iPrevSegment
                       , FOdysseyVectorSegment* iNextSegment );

    protected:
        std::vector<FOdysseyVectorPolygon3> mPolygonCache;
        FOdysseyVectorVertex* mVertex;
        ::ULIS::FRectD mBBox;
        double mLength;
};
