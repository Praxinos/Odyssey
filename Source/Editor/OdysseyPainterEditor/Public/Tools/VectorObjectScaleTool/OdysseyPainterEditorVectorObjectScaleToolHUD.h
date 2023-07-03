#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUDSelection.h"
#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorObjectScaleToolHUD : public FOdysseyVectorHUDSelection
{
    private:
        double mX;
        double mY;
        UOdysseyPainterEditorVectorObjectScaleTool* mObjectScaleTool;
        ::ULIS::FRectD mHandle[4];
        uint32 mHandleFlags;

    public:
        static const uint32 PICK_TOPLEFT     = ( 1      );
        static const uint32 PICK_TOPRIGHT    = ( 1 << 1 );
        static const uint32 PICK_BOTTOMRIGHT = ( 1 << 2 );
        static const uint32 PICK_BOTTOMLEFT  = ( 1 << 3 );
        static const uint32 PICK_CHANGED     = ( 1 << 4 );
        static const uint32 HANDLE_MASK      = ( PICK_TOPLEFT
                                               | PICK_TOPRIGHT
                                               | PICK_BOTTOMRIGHT
                                               | PICK_BOTTOMLEFT );
        static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorObjectScaleToolHUD();
        FOdysseyPainterEditorVectorObjectScaleToolHUD( UOdysseyPainterEditorVectorObjectScaleTool* iObjectScaleTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        uint32 GetHandleFlags();
        uint32 SetCursorPosition( double iX, double iY );

    private:
        void PickHandle( double iWorldX, double iWorldY );
};
