#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDPicking : public FOdysseyVectorHUD
{
    private:
        double mX;
        double mY;
        double mRadius;

    public:
        virtual ~FOdysseyVectorHUDPicking();
        FOdysseyVectorHUDPicking();

        void Draw( FOdysseyVectorScene* iScene, uint64 iFlags );
        void SetRadius( double iRadius );
        void SetPosition( double iX, double iY );
};
