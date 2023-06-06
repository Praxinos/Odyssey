#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDPathPush : public FOdysseyVectorHUD
{
    private:
        static inline ::ULIS::FRectD POINTRECT  = { -1, -1, 2, 2 };
        void DrawObjectRecursive( FOdysseyVectorObject* iObj, BLContext* iBLCtx );

    public:
        ~FOdysseyVectorHUDPathPush();
        FOdysseyVectorHUDPathPush();
        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

    protected:
        bool mRestrictToSelection;
};
