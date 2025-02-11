// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathStitchToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathStitchToolHUD();
        FOdysseyPainterEditorVectorPathStitchToolHUD(  UOdysseyPainterEditorVectorPathStitchTool* iPathStitchTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        void SetPosition( double iWorldX, double iWorldY );
        FOdysseyVectorVertex** GetStitchableVertices();

    protected:
        ::ULIS::FVec2D mPoint[2];
        UOdysseyPainterEditorVectorPathStitchTool* mPathStitchTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
        FOdysseyVectorVertex* mStitchableVertex[2];
        double mX;
        double mY;
};
