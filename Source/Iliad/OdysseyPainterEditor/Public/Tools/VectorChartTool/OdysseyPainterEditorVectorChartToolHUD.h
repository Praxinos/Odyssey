// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"

class FOdysseyVectorTagInbetweener;
class FInbetweenerBreakdown;
struct FInbetweenerDrawing;
struct FChartDivision;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorChartToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    struct FGlyph
    {
        char str[6];
        ::ULIS::FRectI bbox;

        FGlyph()
            : bbox ( 0, 0, 0, 0 )
        {
        }
    };

    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorChartToolHUD();
        FOdysseyPainterEditorVectorChartToolHUD(  UOdysseyPainterEditorVectorChartTool* iGridTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        FChartDivision* PickInbetween( double iWorldX
                                      , double iWorldY );
        void MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                          , FChartDivision* iInbetween
                          , double iWorldX
                          , double iWorldY
                          , bool iRelative );

        void UpdateBreakdown( FOdysseyVectorGroupPaint* iScene );
        FInbetweenerBreakdown* GetBreakdown();
        ::ULIS::FVec2D* PickBezierPoint( double iWorldX
                                       , double iWorldY
                                       , double iRadius );
        const FGlyph* GetGlyph( uint32 iNum );

    private:
        void DrawChart( BLContext* iBLContext
                      , BLRgba32& iFgColor
                      , BLRgba32& iBgColor
                      , BLRgba32& iHcColor
                      , FInbetweenerBreakdown* iBreakdown
                      , uint32 iRenderedCellIndex );

    private:
        UOdysseyPainterEditorVectorChartTool* mChartTool;
        FInbetweenerBreakdown* mBreakdown;
        ::ULIS::FRectD mChartRect;
        BLFont mFont;
        std::vector<FGlyph> mGlyphBuffer;
};
