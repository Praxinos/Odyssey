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

        virtual void Load( FOdysseyVectorGroupPaint* iScene );
        virtual void Unload( FOdysseyVectorGroupPaint* iScene );
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Draw( BLContext* iBLContext
                         , FOdysseyVectorGroupPaint* iScene ) override;

        void DrawObjects( BLContext* iBLContext
                        , FOdysseyVectorGroupPaint* iScene
                        , const BLRgba32& iForegroundColor
                        , const BLRgba32& iBackgroundColor
                        , const BLRgba32& iHighlightColor
                        , uint64 iHUDFlags );

        FSelectionBox& GetSelectionBox();

        std::list<FInbetweenerBreakdown*>& GetSelectedBreakdownList();
        std::list<FOdysseyVectorTagInbetweener*>& GetSelectedInbetweenerTagList();

    protected:
        void UpdateSelectionBoxVertexMode( FOdysseyVectorGroupPaint* iScene );

        void UpdateSelectionBoxObjectMode( FOdysseyVectorGroupPaint* iScene
                                         , bool iForceWorld );
        void UpdateSelectionBoxInbetweenMode( FOdysseyVectorGroupPaint* iScene
                                            , bool iForceWorld );
        void UpdateSelectionBox( FOdysseyVectorGroupPaint* iScene
                               , bool iForceWorld
                               , uint64 iHUDFlags );

        void DrawSelectionBox( BLContext* iBLContext
                             , FOdysseyVectorGroupPaint* iScene
                             , BLRgba32& iForegroundColor
                             , BLRgba32& iBackgroundColor
                             , BLRgba32& iHighlightColor
                             , uint64 iHUDFlags );
        void UpdateSelectionInbetweenMode( FOdysseyVectorGroupPaint* iScene );

    protected:
        UOdysseyPainterEditorVectorBaseTool* mBaseTool;
        FSelectionBox mSelectionBox;
        std::list<FInbetweenerBreakdown*> mSelectedBreakdownList;
        std::list<FOdysseyVectorTagInbetweener*> mSelectedInbetweenerTagList;
};
