#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUDSelection.h"
#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorObjectRotateToolHUD : public FOdysseyVectorHUDSelection
{
    public:
        virtual ~FOdysseyPainterEditorVectorObjectRotateToolHUD();
        FOdysseyPainterEditorVectorObjectRotateToolHUD( UOdysseyPainterEditorVectorObjectRotateTool* iObjectRotateTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        bool PickPivot( double iWorldX, double iWorldY );
        ::ULIS::FVec2D& GetPivot();
        void SetPivot( double iLocalX, double iLocalY );

    private:
        static const uint32 PIVOT_RADIUS = 10;
        ::ULIS::FVec2D mPivot;
        UOdysseyPainterEditorVectorObjectRotateTool* mObjectRotateTool;
        BLFont mFont;
};
