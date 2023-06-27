#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUD.h"

typedef struct _FSelectionBox
{
    ::ULIS::FRectD rect;
    BLMatrix2D worldMatrix;
    BLMatrix2D inverseWorldMatrix;
} FSelectionBox;

class ODYSSEYVECTOR_API FOdysseyVectorHUDSelection : public FOdysseyVectorHUD
{
    protected:
        std::vector<::ULIS::FVec2D>* mPointArray;
        bool mSelecting;
        BLImage* mSelectionMask;
        void DrawSelectionSpace( FOdysseyVectorScene* iScene, uint64 iFlags );
        FSelectionBox mSelectionBox;

    protected:
        void DrawSelectionBox( FOdysseyVectorScene* iScene, uint64 iFlags );
        void DrawSelecting( FOdysseyVectorScene* iScene, uint64 iFlags );

    public:
        virtual ~FOdysseyVectorHUDSelection();
        FOdysseyVectorHUDSelection();

        FSelectionBox& GetSelectionBox();
        void Init( uint32 iWidth, uint32 iHeight );
        virtual void UpdateSelectionBox( FOdysseyVectorScene* iScene, bool iForceWorld );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        void SetSelecting( bool iSelecting, std::vector<::ULIS::FVec2D>* iPointArray );
};
