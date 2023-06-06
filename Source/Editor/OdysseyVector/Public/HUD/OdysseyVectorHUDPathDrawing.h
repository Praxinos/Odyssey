#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDPathDrawing : public FOdysseyVectorHUD
{
    private:
        double mX;
        double mY;
        double mRadius;
        double mStitchingRadius;

    public:
        virtual ~FOdysseyVectorHUDPathDrawing();
        FOdysseyVectorHUDPathDrawing();

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        void SetRadius( double iRadius );
        void SetStitchingRadius( double iRadius );
        void SetPosition( double iX, double iY );
};
