#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUDSelection.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPickToolHUD : public FOdysseyVectorHUDSelection
{
    public:
        virtual ~FOdysseyPainterEditorVectorPickToolHUD();
        FOdysseyPainterEditorVectorPickToolHUD(  UOdysseyPainterEditorVectorPickTool* iPickTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

    protected:
        void DrawSelectionPolygon( BLContext* iBLContext
                                 , BLRgba32 fgColor
                                 , BLRgba32 bgColor
                                 , BLRgba32 hcColor );
        void DrawVertexSelection( FOdysseyVectorScene* iScene, uint64 iFlags );

    private:
        UOdysseyPainterEditorVectorPickTool* mPickTool;
};
