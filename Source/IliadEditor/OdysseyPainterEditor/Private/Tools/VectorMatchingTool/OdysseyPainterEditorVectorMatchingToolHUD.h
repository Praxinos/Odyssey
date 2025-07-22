// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"

class FOdysseyVectorTagInbetweener;
struct FInbetweenerInbetween;
class FInbetweenerPoint;
class FInbetweenerGrid;
class FInbetweenerBreakdown;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorMatchingToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorMatchingToolHUD();
        FOdysseyPainterEditorVectorMatchingToolHUD(  UOdysseyPainterEditorVectorMatchingTool* iGridTool );

        virtual void DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        void PickTargetPoints( FInbetweenerBreakdown* iBreakdown
                             , double iWorldX
                             , double iWorldY
                             , double iRadius
                             , std::vector<FInbetweenerPoint*>& oPointArray
                             , std::vector<float>& oDistanceArray
                             , std::vector<FInbetweenerGrid*>& oGridArray );

    private:
        void DrawPaths( BLContext* iBLContext, FOdysseyVectorTagInbetweener* iInbetweenerTag );
        void DrawTargetGrid( BLContext* iBLContext
                           , FInbetweenerBreakdown* iBreakdown
                           , const BLRgba32& iFgColor
                           , const BLRgba32& iBgColor
                           , const BLRgba32& iHcColor );
        void UpdateBreakdown();

    private:
        UOdysseyPainterEditorVectorMatchingTool* mMatchingTool;
        ::ULIS::FRectD mChartRect;
};
