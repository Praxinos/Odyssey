#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDBucket : public FOdysseyVectorHUD
{
    private:

    public:
        ~FOdysseyVectorHUDBucket();
        FOdysseyVectorHUDBucket();

        void Draw( UOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
