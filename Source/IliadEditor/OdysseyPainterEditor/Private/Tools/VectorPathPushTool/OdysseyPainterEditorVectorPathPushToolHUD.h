// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

        void DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams );
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

        void SetCursorPosition( double iX, double iY );
};
