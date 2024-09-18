#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"

class FOdysseyVectorTagInbetweener;
class FInbetweenerBreakdown;
struct FInbetweenerDrawing;
struct FChartInbetween;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorChartToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorChartToolHUD();
        FOdysseyPainterEditorVectorChartToolHUD(  UOdysseyPainterEditorVectorChartTool* iGridTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        FChartInbetween* PickInbetween( double iWorldX
                                      , double iWorldY
                                      , double iRadius );
        void MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                          , FChartInbetween* iInbetween
                          , double iWorldX
                          , double iWorldY
                          , bool iRelative );

        void UpdateBreakdown( FOdysseyVectorGroupPaint* iScene );
        FInbetweenerBreakdown* GetBreakdown();

    private:
        void DrawChart( BLContext* iBLContext
                      , BLRgba32& iFgColor
                      , BLRgba32& iBgColor
                      , BLRgba32& iHcColor
                      , FInbetweenerBreakdown* iBreakdown );

    private:
        UOdysseyPainterEditorVectorChartTool* mChartTool;
        FInbetweenerBreakdown* mBreakdown;
        ::ULIS::FRectD mChartRect;
        BLFont mFont;
};
