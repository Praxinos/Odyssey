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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        virtual bool IsObjectDisplayed( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorObject* iObject
                                      , uint64 iHUDFlags );

        virtual bool IsObjectAltered( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorObject* iObject
                                    , uint64 iHUDFlags );

        void SetPosition( double iWorldX, double iWorldY );
        std::vector<FOdysseyVectorPoint*>& GetPickedPointArray();

    protected:
        ::ULIS::FVec2D mPoint[2];
        UOdysseyPainterEditorVectorPathStitchTool* mPathStitchTool;
        std::vector<FOdysseyVectorPoint*> mPickedPointArray;
       double mX;
       double mY;
};
