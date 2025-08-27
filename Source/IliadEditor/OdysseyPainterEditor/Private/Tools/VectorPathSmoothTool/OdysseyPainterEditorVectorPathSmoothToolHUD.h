// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathSmoothToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathSmoothToolHUD();
        FOdysseyPainterEditorVectorPathSmoothToolHUD( UOdysseyPainterEditorVectorPathSmoothTool* iPathSmoothTool );

        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

        virtual void SetCursorPosition( double iX, double iY ) override;

        std::vector<FOdysseyVectorPoint*>& GetPickedPointArray();

    private:
        UOdysseyPainterEditorVectorPathSmoothTool* mPathSmoothTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
};
