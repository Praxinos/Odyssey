#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathPushToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    private:
        UOdysseyPainterEditorVectorPathPushTool* mPathPushTool;
        double mX;
        double mY;

    public:
        virtual ~FOdysseyPainterEditorVectorPathPushToolHUD();
        FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        void SetCursorPosition( double iX, double iY );
};
