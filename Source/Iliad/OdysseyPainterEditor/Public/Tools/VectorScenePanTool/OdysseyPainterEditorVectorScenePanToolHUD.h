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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        void SetFactor( double iFactor );
    private:
        void DrawFrame( BLContext* iBLContext
                      , FOdysseyVectorGroupPaint* iScene
                      , ::ULIS::FRectD& iFrame
                      , ::ULIS::FVec2D& iFrameLength );
        void DrawText( BLContext* iBLContext
                     , FOdysseyVectorGroupPaint* iScene
                     , ::ULIS::FRectD& iFrame );
};
