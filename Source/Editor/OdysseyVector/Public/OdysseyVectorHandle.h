#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandle : public FOdysseyVectorPoint
{
    private:

    protected:

    public:
        ~FOdysseyVectorHandle();
        FOdysseyVectorHandle();
        FOdysseyVectorHandle( double iX, double iY );
};
