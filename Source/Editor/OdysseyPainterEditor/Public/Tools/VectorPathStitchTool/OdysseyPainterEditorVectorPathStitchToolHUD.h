#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathStitchToolHUD : public FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathStitchToolHUD();
        FOdysseyPainterEditorVectorPathStitchToolHUD(  UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        void SetPosition( double iWorldX, double iWorldY );
        std::vector<FOdysseyVectorPoint*>& GetPickedPointArray();

    protected:
        ::ULIS::FVec2D mPoint[2];
        UOdysseyPainterEditorVectorPathStitchTool* mPathStitchTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
       double mX;
       double mY;
};
