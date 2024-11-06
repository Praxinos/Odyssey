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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

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
