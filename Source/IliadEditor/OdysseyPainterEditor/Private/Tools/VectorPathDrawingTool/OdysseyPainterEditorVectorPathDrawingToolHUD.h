// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;

        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;

        virtual void SetCursorPosition( double iWorldX, double iWorldY ) override;
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
