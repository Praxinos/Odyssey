#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPathEditToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPathEditToolHUD(  );
        FOdysseyPainterEditorVectorPathEditToolHUD( UOdysseyPainterEditorVectorPathEditTool* iPathEditTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;
        void ClearMask();
        ::ULIS::FRectD GenerateMask( double iX
                                   , double iY
                                   , double iRadius );
        BLImage* GetMask();

        bool SetCursorPosition( double iX, double iY );

    private:
        double mX;
        double mY;
        UOdysseyPainterEditorVectorPathEditTool* mPathEditTool;
        BLContext mBLSelectionContext;
        BLImage mBLSelectionMask;

};
