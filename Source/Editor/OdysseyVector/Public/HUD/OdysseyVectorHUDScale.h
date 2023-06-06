#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUDSelection.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDScale : public FOdysseyVectorHUDSelection
{
    private:


    protected:
        ::ULIS::FRectD mHandle[4];

    public:
        static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyVectorHUDScale();
        FOdysseyVectorHUDScale();

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        int32 Pick( double iWorldX, double iWorldY );
};
