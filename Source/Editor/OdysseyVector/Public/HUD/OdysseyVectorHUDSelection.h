#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorHUDSelection : public FOdysseyVectorHUD
{
    private:
        std::vector<::ULIS::FVec2D>& mPointArray;
        bool mSelecting;
        BLImage* mSelectionMask;
        void DrawSelectionSpace( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );

    public:
        ~FOdysseyVectorHUDSelection();
        FOdysseyVectorHUDSelection( std::vector<::ULIS::FVec2D>& iPointArray );

        void Init( uint32 iWidth, uint32 iHeight );
        void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
        void SetSelecting( bool iSelecting );
};
