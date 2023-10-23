#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathCutToolHUD : public FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathCutToolHUD();
        FOdysseyPainterEditorVectorPathCutToolHUD(  UOdysseyPainterEditorVectorPathCutTool* iPathCutTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        void SetP0( double iX, double iY );
        void SetP1( double iX, double iY );

        ::ULIS::FVec2D& GetP0();
        ::ULIS::FVec2D& GetP1();

    protected:
        ::ULIS::FVec2D mPoint[2];

    private:
        UOdysseyPainterEditorVectorPathCutTool* mPathCutTool;
};
