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

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        void SetRadius( double iRadius );
        void SetPosition( double iX, double iY );
};
