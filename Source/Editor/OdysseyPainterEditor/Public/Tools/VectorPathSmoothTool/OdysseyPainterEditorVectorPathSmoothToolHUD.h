#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathSmoothToolHUD : public FOdysseyVectorHUD
{


    public:
        virtual ~FOdysseyPainterEditorVectorPathSmoothToolHUD();
        FOdysseyPainterEditorVectorPathSmoothToolHUD( UOdysseyPainterEditorVectorPathSmoothTool* iPathSmoothTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;

        void SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetPickedPointArray();

    private:
        UOdysseyPainterEditorVectorPathSmoothTool* mPathSmoothTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
        double mX;
        double mY;
};
