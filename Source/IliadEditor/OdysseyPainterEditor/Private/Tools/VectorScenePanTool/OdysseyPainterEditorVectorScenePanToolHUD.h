// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorScenePanToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    private:
        UOdysseyPainterEditorVectorScenePanTool* mScenePanTool;
        BLFont mFont;

    public:
        virtual ~FOdysseyPainterEditorVectorScenePanToolHUD();
        FOdysseyPainterEditorVectorScenePanToolHUD( UOdysseyPainterEditorVectorScenePanTool* iScenePanTool );

        void DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams );
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

        void SetFactor( double iFactor );

    private:
        void DrawFrame( const FOdysseyHUDSystem::FDrawHUDParams& iParams
                      , ::ULIS::FRectI& iFrame
                      , ::ULIS::FVec2I& iFrameLength
                      , const FLinearColor& iFgColor
                      , const FLinearColor& iBgColor );

    private:
        FSlateFontInfo mFontInfo;
};
