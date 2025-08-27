// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathStitchToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathStitchToolHUD();
        FOdysseyPainterEditorVectorPathStitchToolHUD(  UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool );

        virtual void Draw( BLContext* iBLContext ) override;
        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;

        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

        virtual void SetCursorPosition( double iWorldX, double iWorldY ) override;

        FOdysseyVectorVertex** GetStitchableVertices();

    protected:
        ::ULIS::FVec2D mPoint[2];
        UOdysseyPainterEditorVectorPathStitchTool* mPathStitchTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
        FOdysseyVectorVertex* mStitchableVertex[2];
};
