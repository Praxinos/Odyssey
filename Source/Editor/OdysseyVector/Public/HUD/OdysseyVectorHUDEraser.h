#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDEraser : public FOdysseyVectorHUD
{
    private:

    public:
        ~FOdysseyVectorHUDEraser();
        FOdysseyVectorHUDEraser( std::vector<::ULIS::FVec2D>& iPointArray );

        void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
