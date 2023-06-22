#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathPushToolHUD : public FOdysseyVectorHUD
{
    private:
        UOdysseyPainterEditorVectorPathPushTool* mPathPushTool;
        double mX;
        double mY;

    public:
        virtual ~FOdysseyPainterEditorVectorPathPushToolHUD();
        FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        void SetCursorPosition( double iX, double iY );

    private:
        void DrawObjectRecursive( FOdysseyVectorObject* iObj, BLContext* iBLCtx );
};
