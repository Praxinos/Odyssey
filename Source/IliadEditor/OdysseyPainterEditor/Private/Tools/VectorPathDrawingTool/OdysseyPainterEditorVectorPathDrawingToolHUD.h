// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"

#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathDrawingToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathDrawingToolHUD();

        FOdysseyPainterEditorVectorPathDrawingToolHUD( UOdysseyPainterEditorVectorPathDrawingTool* iPathDrawingTool );

        virtual void DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

        bool SetCursorPosition( double iX, double iY );
        std::vector<FOdysseyVectorPoint*>& GetStitchedPointArray();

    private:
        void DrawEdge( BLContext* iBLContext
                     , FTracerEdge* iPrevEdge
                     , FTracerEdge* iCurrEdge
                     , FTracerEdge* iNextEdge );

    private:
        double mX;
        double mY;
        std::vector<FOdysseyVectorPoint*> mStitchedPointArray;
        UOdysseyPainterEditorVectorPathDrawingTool* mPathDrawingTool;
        FOdysseyVectorObject mOwnerObject;
        FOdysseyVectorVertex mVertex[2];
        FOdysseyVectorSegmentCubic mCubicSegment;
};
