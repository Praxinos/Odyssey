#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUDSelection.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorTransformToolHUD : public FOdysseyVectorHUDSelection
{
    public:
        static const uint32 PICK_XAXIS              = ( 1      );
        static const uint32 PICK_YAXIS              = ( 1 << 1 );
        static const uint32 PICK_ZAXIS              = ( 1 << 2 );
        static const uint32 PICK_TRANSLATE          = ( 1 << 3 );
        static const uint32 PICK_ROTATE             = ( 1 << 4 );
        static const uint32 PICK_CHANGED            = ( 1 << 5 );
        static const uint32 PICK_SCALER_TOPLEFT     = ( 1 << 6 );
        static const uint32 PICK_SCALER_TOPRIGHT    = ( 1 << 7 );
        static const uint32 PICK_SCALER_BOTTOMRIGHT = ( 1 << 8 );
        static const uint32 PICK_SCALER_BOTTOMLEFT  = ( 1 << 9 );

        virtual ~FOdysseyPainterEditorVectorTransformToolHUD();
        FOdysseyPainterEditorVectorTransformToolHUD(  UOdysseyPainterEditorVectorTransformTool* iTransformTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        uint32 SetCursorPosition( double iWorldX, double iWorldY );
        void SetGizmo( double iLocalX, double iLocalY );
        ::ULIS::FVec2D& GetGizmo();
        uint32 GetFlags();
        void ShowSelectionBox( bool iShowSelectionBox );

    private:
       uint32 PickScalers( double iWorldX, double iWorldY );
        uint32 PickSelectionBox( double iWorldX, double iWorldY );
        uint32 PickGizmo( double iWorldX, double iWorldY );
        void Pick( double iWorldX, double iWorldY );
        void UpdateGizmo();
        void DrawGizmo( FOdysseyVectorScene* iScene, uint64 iFlags );
        void DrawScalers( FOdysseyVectorScene* iScene, uint64 iFlags );

    private:
        UOdysseyPainterEditorVectorTransformTool* mTransformTool;
        static const uint32 GIZMO_RADIUS = 10;
        static const uint32 SCALER_RADIUS  = 5;
        ::ULIS::FVec2D mGizmo;
        uint32 mFlags;
        ::ULIS::FVec2D mXAxis;
        ::ULIS::FVec2D mYAxis;
        bool mShowSelectionBox;
};
