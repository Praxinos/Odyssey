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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        FSelectionBox& GetSelectionBox();

    protected:
        void DrawObjectModeRecursive( BLContext* iBLContext
                                    , FOdysseyVectorObject* iObject
                                    , BLRgba32& iForegroundColor
                                    , BLRgba32& iBackgroundColor
                                    , BLRgba32& iHighlightColor
                                    , uint64 iDrawingFlags );
        void DrawObjectMode( BLContext* iBLContext
                           , FOdysseyVectorScene* iScene
                           , uint64 iDrawingFlags );

        void DrawVertexModeRecursive( BLContext* iBLContext
                                    , FOdysseyVectorObject* iObject
                                    , BLRgba32& iForegroundColor
                                    , BLRgba32& iBackgroundColor
                                    , BLRgba32& iHighlightColor
                                    , uint64 iDrawingFlags );
        void DrawVertexMode( BLContext* iBLContext
                           , FOdysseyVectorScene* iScene
                           , uint64 iDrawingFlags );

        void DrawSelectionBox( BLContext* iBLContext, FOdysseyVectorScene* iScene, uint64 iFlags );

        void UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene, bool iForceWorld );
        void UpdateSelectionBoxVertexModeRecursive( FOdysseyVectorObject* iObject
                                                  , bool iForceWorld );
        void UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene, bool iForceWorld );
        void UpdateSelectionBox( FOdysseyVectorScene* iScene, bool iForceWorld );

        bool PathGetBBoxFromSelectedVertices( FOdysseyVectorPath* iPath, ::ULIS::FRectD& oBBox );
        bool GroupPaintGetBBoxFromSelectedVertices( FOdysseyVectorGroupPaint* iPaintGroup, ::ULIS::FRectD& oBBox );

    protected:
        UOdysseyPainterEditorVectorBaseTool* mBaseTool;
        FSelectionBox mSelectionBox;
        //bool mSelecting;
};
