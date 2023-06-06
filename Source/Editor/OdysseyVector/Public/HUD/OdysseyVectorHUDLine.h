#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDLine : public FOdysseyVectorHUD
{
    private:
        ::ULIS::FVec2D mP0;
        ::ULIS::FVec2D mP1;

    public:
        virtual ~FOdysseyVectorHUDLine();
        FOdysseyVectorHUDLine();

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        void SetP0( double x, double y );
        void SetP1( double x, double y );
        ::ULIS::FVec2D& GetP0();
        ::ULIS::FVec2D& GetP1();
};
