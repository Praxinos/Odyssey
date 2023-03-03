#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

typedef struct _FSelectionBox
{
    UOdysseyVectorObject* space;
    ::ULIS::FRectD rect;
} FSelectionBox;

class ODYSSEYVECTOR_API FOdysseyVectorHUDSelection : public FOdysseyVectorHUD
{
    protected:
        std::vector<::ULIS::FVec2D>* mPointArray;
        bool mSelecting;
        BLImage* mSelectionMask;
        void DrawSelectionSpace( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
        FSelectionBox mSelectionBox;

    protected:
        void DrawSelectionBox( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
        void DrawSelecting( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );

    public:
        ~FOdysseyVectorHUDSelection();
        FOdysseyVectorHUDSelection();

        FSelectionBox& GetSelectionBox();
        void Init( uint32 iWidth, uint32 iHeight );
        virtual void UpdateSelectionBox( UOdysseyVectorRoot& iScene );
        virtual void Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
        void SetSelecting( bool iSelecting, std::vector<::ULIS::FVec2D>* iPointArray );
};
