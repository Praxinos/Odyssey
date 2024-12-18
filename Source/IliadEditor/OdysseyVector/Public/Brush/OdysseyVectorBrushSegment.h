// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorChain;
class FOdysseyVectorSegment;
typedef struct _FOdysseyVectorFraction FOdysseyVectorFraction;

class FOdysseyVectorBrushSegment
{
    public:
        ~FOdysseyVectorBrushSegment();
        FOdysseyVectorBrushSegment( FOdysseyVectorSegment* iSegment
                                  , const ::ULIS::FRectD& iBoundingBox );

        FOdysseyVectorFraction* GetSegmentFraction( FOdysseyVectorSegment* iSegment
                                                  , double iU );

        void Draw( BLContext* iBLContext
                 , const ::ULIS::FRectD& iInvalidationArea
                 , double iAncestorsOpacity
                 , FOdysseyVectorChain* iChain
                 , uint64 iDrawingFlags );

    private:
        BLPath mBLPath;
};
