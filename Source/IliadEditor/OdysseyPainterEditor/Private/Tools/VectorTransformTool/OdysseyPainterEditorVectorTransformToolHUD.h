// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorTransformToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        static const uint32 AXIS_LENGTH   = 70;
        static const uint32 GIZMO_RADIUS  = 5;
        static const uint32 SCALER_RADIUS = 5;

        static const uint32 PICK_XAXIS              = ( 1       );
        static const uint32 PICK_YAXIS              = ( 1 << 1  );
        static const uint32 PICK_ZAXIS              = ( 1 << 2  );
        static const uint32 PICK_TRANSLATE          = ( PICK_XAXIS
                                                      | PICK_YAXIS );
        static const uint32 PICK_ROTATE             = ( 1 << 4  );
        static const uint32 PICK_CHANGED            = ( 1 << 5  );
        static const uint32 PICK_SCALER_TOPLEFT     = ( 1 << 6  );
        static const uint32 PICK_SCALER_TOPRIGHT    = ( 1 << 7  );
        static const uint32 PICK_SCALER_BOTTOMRIGHT = ( 1 << 8  );
        static const uint32 PICK_SCALER_BOTTOMLEFT  = ( 1 << 9  );
        static const uint32 PICK_SCALE              = ( PICK_SCALER_TOPLEFT
                                                      | PICK_SCALER_TOPRIGHT
                                                      | PICK_SCALER_BOTTOMRIGHT
                                                      | PICK_SCALER_BOTTOMLEFT );

        virtual ~FOdysseyPainterEditorVectorTransformToolHUD();
        FOdysseyPainterEditorVectorTransformToolHUD(  UOdysseyPainterEditorVectorTransformTool* iTransformTool );

        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;

        void SetGizmo( double iLocalX, double iLocalY );
        ::ULIS::FVec2D& GetGizmo();
        uint32 GetFlags();
        uint32 Pick( double iWorldX, double iWorldY );
        void CenterGizmo();
        void SetCenterGizmo( bool iCenterGizmo );

        void ShowSelectionBox( bool iShowSelectionBox );

    private:
        uint32 PickScalers( double iWorldX, double iWorldY );
        uint32 PickSelectionBox( double iWorldX, double iWorldY );
        uint32 PickGizmo( double iWorldX, double iWorldY );
        void UpdateGizmo();
        void DrawGizmo( const FOdysseyHUDElement::FDrawHUDParams& iParams );
        void DrawScalers( const FOdysseyHUDElement::FDrawHUDParams& iParams );

    private:
        UOdysseyPainterEditorVectorTransformTool* mTransformTool;
        ::ULIS::FVec2D mGizmo;
        uint32 mFlags;
        ::ULIS::FVec2D mXAxis;
        ::ULIS::FVec2D mYAxis;
        bool mCenterGizmo;
        bool mShowSelectionBox;
        bool mShowSelectionIfEmpty;
};
