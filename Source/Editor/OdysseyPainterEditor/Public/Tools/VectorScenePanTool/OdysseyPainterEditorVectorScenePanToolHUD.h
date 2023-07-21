#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorScenePanToolHUD : public FOdysseyVectorHUD
{
    private:
        UOdysseyPainterEditorVectorScenePanTool* mScenePanTool;
        BLFont mFont;

    public:
        virtual ~FOdysseyPainterEditorVectorScenePanToolHUD();
        FOdysseyPainterEditorVectorScenePanToolHUD( UOdysseyPainterEditorVectorScenePanTool* iScenePanTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        void SetFactor( double iFactor );
    private:
        void DrawFrame( FOdysseyVectorScene* iScene
                      , ::ULIS::FRectD& iFrame
                      , ::ULIS::FVec2D& iFrameLength );
        void DrawText( FOdysseyVectorScene* iScene, ::ULIS::FRectD& iFrame );
};
