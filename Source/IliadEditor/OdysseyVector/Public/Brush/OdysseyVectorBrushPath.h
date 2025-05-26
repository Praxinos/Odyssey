// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <Brush/OdysseyVectorBrushObject.h>
#include <Brush/OdysseyVectorBrushSegment.h>
#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorChain;
class FOdysseyVectorPath;

class ODYSSEYVECTOR_API FOdysseyVectorBrushPath : public FOdysseyVectorBrushObject
{
    public:
        virtual ~FOdysseyVectorBrushPath();
        FOdysseyVectorBrushPath( FOdysseyVectorPath* iPath
                               , const ::ULIS::FRectD& iBoundingBox );

        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , FOdysseyVectorChain* iChain
                         , uint64 iDrawingFlags ) override ;

    private:
        std::vector<FOdysseyVectorBrushSegment> mBrushSegmentBuffer;
};
