#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDSelection : public FOdysseyVectorHUD
{
    private:
        std::vector<::ULIS::FVec2D>& mPointArray;

    public:
        ~FOdysseyVectorHUDSelection();
        FOdysseyVectorHUDSelection( std::vector<::ULIS::FVec2D>& iPointArray );

        void Draw( UOdysseyVectorObject* iObject, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
