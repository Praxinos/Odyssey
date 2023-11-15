#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathDrawingToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    private:
        double mX;
        double mY;
        std::vector<FOdysseyVectorPoint*> mStitchedPointArray;
        UOdysseyPainterEditorVectorPathDrawingTool* mPathDrawingTool;

    public:
        virtual ~FOdysseyPainterEditorVectorPathDrawingToolHUD();

        FOdysseyPainterEditorVectorPathDrawingToolHUD( UOdysseyPainterEditorVectorPathDrawingTool* iPathDrawingTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        bool SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetStitchedPointArray();
};
