#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandle : public FOdysseyVectorPoint
{
    public:
        void Init( double iX, double iY );

    protected:

    public:
        ~FOdysseyVectorHandle();
        FOdysseyVectorHandle();
};
