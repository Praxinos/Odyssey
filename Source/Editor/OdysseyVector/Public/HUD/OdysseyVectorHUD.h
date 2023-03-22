#pragma once

#include <blend2d.h>
#include <ULIS>

#include "OdysseyVectorObject.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyVectorHUD();
        FOdysseyVectorHUD();

        virtual void Draw( FOdysseyVectorScene* iScene, ::ULIS::FRectD& iRoi, uint64 iFlags ) = 0;
};
