#pragma once

#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorChain;

class ODYSSEYVECTOR_API FOdysseyVectorBrushObject
{
    public:
        virtual ~FOdysseyVectorBrushObject();
        FOdysseyVectorBrushObject();

        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , FOdysseyVectorChain* iChain
                         , uint64 iDrawingFlags ) = 0;
};
