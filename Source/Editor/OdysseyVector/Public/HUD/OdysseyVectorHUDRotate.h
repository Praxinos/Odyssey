#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUDSelection.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDRotate : public FOdysseyVectorHUDSelection
{
    private:


    protected:
        ::ULIS::FVec2D mPivot;
        bool mShowBox;

    public:
        static const uint32 PIVOT_RADIUS = 20;
        //static const uint32 HANDLE_RADIUS = 5;

        ~FOdysseyVectorHUDRotate();
        FOdysseyVectorHUDRotate();

        void SetShowBox( bool iShowBox );
        bool PickPivot( double iWorldX, double iWorldY );
        ::ULIS::FVec2D& GetPivot();
        void SetPivot( double iLocalX, double iLocalY );
        void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
};
