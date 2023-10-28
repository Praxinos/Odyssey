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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        void SetFactor( double iFactor );
    private:
        void DrawFrame( BLContext* iBLContext
                      , FOdysseyVectorScene* iScene
                      , ::ULIS::FRectD& iFrame
                      , ::ULIS::FVec2D& iFrameLength );
        void DrawText( BLContext* iBLContext
                     , FOdysseyVectorScene* iScene
                     , ::ULIS::FRectD& iFrame );
};
