#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"

typedef struct _FSelectionBox
{
    ::ULIS::FRectD rect;
    BLMatrix2D worldMatrix;
    BLMatrix2D inverseWorldMatrix;
} FSelectionBox;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPickToolHUD : public FOdysseyVectorHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPickToolHUD();
        FOdysseyPainterEditorVectorPickToolHUD(  UOdysseyPainterEditorVectorPickTool* iPickTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        void ShowSelectionBox( bool iShowSelectionBox );
        FSelectionBox& GetSelectionBox();
        void Init( uint32 iWidth, uint32 iHeight );
        void GetSelectedVertices( FOdysseyVectorScene* iScene, std::vector<FOdysseyVectorPoint*>& oPointArray );

    protected:
        void DrawPickingArea( BLContext* iBLContext
                            , BLRgba32 fgColor
                            , BLRgba32 bgColor
                            , BLRgba32 hcColor );
        void DrawVertexSelection( FOdysseyVectorScene* iScene, uint64 iFlags );
        void DrawObjectSelection( FOdysseyVectorScene* iScene, uint64 iFlags );
        void DrawSelectionSpace( FOdysseyVectorScene* iScene, uint64 iFlags );
        void UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene, bool iForceWorld );
        void UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene, bool iForceWorld );
        void UpdateSelectionBox( FOdysseyVectorScene* iScene, bool iForceWorld );
        void DrawSelectionBox( FOdysseyVectorScene* iScene, uint64 iFlags );

        bool PathGetBBoxFromSelectedVertices( FOdysseyVectorPath* iPath, ::ULIS::FRectD& oBBox );
        bool GroupPaintGetBBoxFromSelectedVertices( FOdysseyVectorGroupPaint* iPaintGroup, ::ULIS::FRectD& oBBox );

    protected:
        UOdysseyPainterEditorVectorPickTool* mPickTool;
        std::vector<::ULIS::FVec2D>* mPointArray;
        //bool mSelecting;
        BLImage* mSelectionMask;
        FSelectionBox mSelectionBox;
        bool mShowSelectionBox;
};
