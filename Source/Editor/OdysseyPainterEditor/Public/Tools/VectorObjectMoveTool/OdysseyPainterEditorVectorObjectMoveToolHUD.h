#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUDSelection.h"
#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorObjectMoveToolHUD : public FOdysseyVectorHUDSelection
{
    private:
        double mX;
        double mY;
        UOdysseyPainterEditorVectorObjectMoveTool* mObjectMoveTool;
        ::ULIS::FVec2D mXAxis;
        ::ULIS::FVec2D mYAxis;
        ::ULIS::FVec2D mPivot;
        uint32 mGizmoFlags;

    public:
        static const uint32 PICK_XAXIS   = ( 1      );
        static const uint32 PICK_YAXIS   = ( 1 << 1 );
        static const uint32 PICK_CHANGED = ( 1 << 2 );

    private:
        void DrawGizmo( FOdysseyVectorScene* iScene );

    public:
        virtual ~FOdysseyPainterEditorVectorObjectMoveToolHUD();
        FOdysseyPainterEditorVectorObjectMoveToolHUD(){};
        FOdysseyPainterEditorVectorObjectMoveToolHUD( UOdysseyPainterEditorVectorObjectMoveTool* iObjectMoveTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;

        uint32 GetGizmoFlags();
        uint32 SetCursorPosition( double iX, double iY );

    private:
        void PickGizmo( double iWorldX, double iWorldY );
        void UpdateGizmo();
};
