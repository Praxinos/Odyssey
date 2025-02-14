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
class Inbetween;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorChartToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorChartToolHUD();
        FOdysseyPainterEditorVectorChartToolHUD(  UOdysseyPainterEditorVectorChartTool* iGridTool );

        virtual void Load() override;
        virtual void DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        FInbetweenerChart::Inbetween* PickInbetween( double iWorldX
                                                   , double iWorldY );
        void MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                          , FInbetweenerChart::Inbetween* iInbetween
                          , double iWorldX
                          , double iWorldY
                          , bool iRelative );

        void UpdateBreakdown( );
        std::list<FInbetweenerBreakdown*>& GetBreakdownList();
        FInbetweenerChart::HUDBezier::Point* PickBezierPoint( double iWorldX
                                                            , double iWorldY
                                                            , double iRadius );
        FInbetweenerBreakdown* PickBreakdown( double iWorldX
                                            , double iWorldY
                                            , double iRadius );

    private:
        void DrawBreakdownChart( const FOdysseyHUDSystem::FDrawHUDParams& iParams
                               , const FLinearColor& iFgColor
                               , const FLinearColor& iBgColor
                               , const FLinearColor& iHcColor
                               , FInbetweenerBreakdown* iBreakdown
                               , FInbetweenerChart::HUDBezier* iHUDBezier
                               , uint32 iRenderedCellIndex
                               , bool iDrawSourceIndicator );

    private:
        UOdysseyPainterEditorVectorChartTool* mChartTool;
        std::list<FInbetweenerBreakdown*> mBreakdownList;
        ::ULIS::FRectD mChartRect;
        FSlateFontInfo mFontInfo;
};
