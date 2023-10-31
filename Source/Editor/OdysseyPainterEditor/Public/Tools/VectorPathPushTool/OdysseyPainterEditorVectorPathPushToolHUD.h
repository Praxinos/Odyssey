#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathPushToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    private:
        UOdysseyPainterEditorVectorPathPushTool* mPathPushTool;
        double mX;
        double mY;

    public:
        virtual ~FOdysseyPainterEditorVectorPathPushToolHUD();
        FOdysseyPainterEditorVectorPathPushToolHUD( UOdysseyPainterEditorVectorPathPushTool* iPathPushTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        virtual bool IsObjectDisplayed( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorObject* iObject
                                      , uint64 iHUDFlags ) override;

        virtual bool IsObjectAltered( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorObject* iObject
                                    , uint64 iHUDFlags ) override;

        void SetCursorPosition( double iX, double iY );
};
