#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDTransform : public FOdysseyVectorHUD
{
    private:

    public:
        ~FOdysseyVectorHUDTransform();
        FOdysseyVectorHUDTransform( std::vector<::ULIS::FVec2D>& iPointArray );

        void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
