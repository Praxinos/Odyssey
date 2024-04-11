#pragma once

#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorChain;
class FOdysseyVectorSegment;

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
