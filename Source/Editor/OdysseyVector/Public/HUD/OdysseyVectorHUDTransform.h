#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDTransform : public FOdysseyVectorHUD
{
    private:
        static const uint32 HANDLE_RADIUS = 3;

    protected:
        ::ULIS::FRectD mRect;

    public:
        ~FOdysseyVectorHUDTransform();
        FOdysseyVectorHUDTransform();

        void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
