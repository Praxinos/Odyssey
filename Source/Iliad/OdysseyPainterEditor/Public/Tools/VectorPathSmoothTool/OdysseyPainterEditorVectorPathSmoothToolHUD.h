// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        void SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetPickedPointArray();

    private:
        UOdysseyPainterEditorVectorPathSmoothTool* mPathSmoothTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
        double mX;
        double mY;
};
