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

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        void SetCycle( FOdysseyVectorCycle* iCycle );
};
