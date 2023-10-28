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

        virtual void Draw( BLContext* iBLContext
                         , FOdysseyVectorScene* iScene
                         , uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        bool SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetStitchedPointArray();
};
