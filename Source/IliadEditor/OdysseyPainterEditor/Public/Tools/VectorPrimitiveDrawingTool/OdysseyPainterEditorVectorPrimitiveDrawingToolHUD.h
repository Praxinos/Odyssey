// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD();
        FOdysseyPainterEditorVectorPrimitiveDrawingToolHUD( UOdysseyPainterEditorVectorPrimitiveDrawingTool* iPrimitiveDrawingTool );

        void DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams );
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

    private:
        void DrawFrame( const FOdysseyHUD::FDrawHUDParams& iParams
                      , ::ULIS::FRectI& iFrame
                      , ::ULIS::FVec2I& iFrameLength
                      , const FLinearColor& iFgColor
                      , const FLinearColor& iBgColor );

    private:
        UOdysseyPainterEditorVectorPrimitiveDrawingTool* mPrimitiveDrawingTool;
};
