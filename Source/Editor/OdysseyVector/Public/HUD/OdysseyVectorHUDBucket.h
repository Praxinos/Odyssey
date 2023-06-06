#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDBucket : public FOdysseyVectorHUD
{
    private:
        FOdysseyVectorCycle* mCycle;

    public:
        ~FOdysseyVectorHUDBucket();
        FOdysseyVectorHUDBucket();

        void Draw( FOdysseyVectorScene* iScene, uint64 iFlags );
        void SetCycle( FOdysseyVectorCycle* iCycle );
};
