#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorBaseToolHUD : public FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorBaseToolHUD();
        FOdysseyPainterEditorVectorBaseToolHUD(  UOdysseyPainterEditorVectorBaseTool* iBaseTool );

        virtual void Load( FOdysseyVectorScene* iScene ) = 0;
        virtual void Unload( FOdysseyVectorScene* iScene )  = 0;

    protected:
        uint64 GetViewingMode();
        virtual bool IsTargetObject( FOdysseyVectorScene* iScene
                                   , FOdysseyVectorObject* iObject
                                   , uint64 iHUDFlags ) override;
    protected:
        UOdysseyPainterEditorVectorBaseTool* mBaseTool;
        //bool mSelecting;
};
