// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathDrawingToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    private:
        double mX;
        double mY;
        std::vector<FOdysseyVectorPoint*> mStitchedPointArray;
        UOdysseyPainterEditorVectorPathDrawingTool* mPathDrawingTool;

        void DrawEdge( BLContext* iBLContext
                     , FTracerEdge* iPrevEdge
                     , FTracerEdge* iCurrEdge
                     , FTracerEdge* iNextEdge );

    public:
        virtual ~FOdysseyPainterEditorVectorPathDrawingToolHUD();

        FOdysseyPainterEditorVectorPathDrawingToolHUD( UOdysseyPainterEditorVectorPathDrawingTool* iPathDrawingTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        bool SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetStitchedPointArray();
};
