#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseTool.h"

typedef struct _FSelectionBox
{
    bool inited;
    ::ULIS::FRectD rect;
    BLMatrix2D worldMatrix;
    BLMatrix2D inverseWorldMatrix;
} FSelectionBox;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorBaseToolHUD : public FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorBaseToolHUD();
        FOdysseyPainterEditorVectorBaseToolHUD(  UOdysseyPainterEditorVectorBaseTool* iBaseTool );

        virtual void Load( FOdysseyVectorScene* iScene ) = 0;
        virtual void Unload( FOdysseyVectorScene* iScene )  = 0;

        void DrawObjects( BLContext* iBLContext
                        , FOdysseyVectorScene* iScene
                        , const BLRgba32& iForegroundColor
                        , const BLRgba32& iBackgroundColor
                        , const BLRgba32& iHighlightColor
                        , uint64 iHUDFlags );

        FSelectionBox& GetSelectionBox();

    protected:
        void UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene
                                         , uint64 iHUDFlags );

        void UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene
                                         , bool iForceWorld
                                         , uint64 iHUDFlags );

        void UpdateSelectionBox( FOdysseyVectorScene* iScene
                               , bool iForceWorld
                               , uint64 iHUDFlags );

        void DrawSelectionBox( BLContext* iBLContext
                             , FOdysseyVectorScene* iScene
                             , BLRgba32& iForegroundColor
                             , BLRgba32& iBackgroundColor
                             , BLRgba32& iHighlightColor
                             , uint64 iHUDFlags );

    protected:
        UOdysseyPainterEditorVectorBaseTool* mBaseTool;
        FSelectionBox mSelectionBox;
        //bool mSelecting;
};
