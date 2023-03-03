#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUDSelection.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDTransform : public FOdysseyVectorHUDSelection
{
    private:


    protected:
        ::ULIS::FRectD mHandle[4];

    public:
        static const uint32 HANDLE_RADIUS = 4;

        ~FOdysseyVectorHUDTransform();
        FOdysseyVectorHUDTransform();

        void UpdateSelectionBox( UOdysseyVectorRoot& iScene );
        void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
        int32 Pick( double iWorldX, double iWorldY );
};
