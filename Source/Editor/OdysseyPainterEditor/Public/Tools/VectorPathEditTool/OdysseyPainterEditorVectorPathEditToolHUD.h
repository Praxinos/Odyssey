#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathEditToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    private:
        double mX;
        double mY;
        UOdysseyPainterEditorVectorPathEditTool* mPathEditTool;

    public:
        virtual ~FOdysseyPainterEditorVectorPathEditToolHUD(  );
        FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool );

        virtual void Draw( BLContext* iBLContext
                         , FOdysseyVectorScene* iScene
                         , uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        bool SetCursorPosition( double iX, double iY );
};
