#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandle : public FOdysseyVectorPoint
{
    public:
        virtual ~FOdysseyVectorHandle();
        FOdysseyVectorHandle();

        void Init( double iX, double iY );
        ::ULIS::FVec2D& GetCoords();
};
