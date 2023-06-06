#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDEraser : public FOdysseyVectorHUD
{
    private:
        double mX;
        double mY;
        double mRadius;
        bool mBlending;

    public:
        ~FOdysseyVectorHUDEraser();
        FOdysseyVectorHUDEraser();

        void Draw( FOdysseyVectorScene* iScene, uint64 iFlags );

        void SetRadius( double iRadius );
        void SetPosition( double iX, double iY );
        void BlendMask( bool iBlending );
};
