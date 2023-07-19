#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathEditToolHUD : public FOdysseyVectorHUD
{
    private:
        double mX;
        double mY;
        UOdysseyPainterEditorVectorPathEditTool* mPathEditTool;

    public:
        virtual ~FOdysseyPainterEditorVectorPathEditToolHUD(  );
        FOdysseyPainterEditorVectorPathEditToolHUD(){};
        FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        bool SetCursorPosition( double iX, double iY );
};
