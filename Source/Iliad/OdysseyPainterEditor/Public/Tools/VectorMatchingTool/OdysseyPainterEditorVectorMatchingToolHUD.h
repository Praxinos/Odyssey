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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        void PickTargetPoints( FInbetweenerBreakdown* iBreakdown
                             , double iWorldX
                             , double iWorldY
                             , double iRadius
                             , std::vector<FInbetweenerPoint*>& oPointArray
                             , std::vector<float>& oDistanceArray
                             , std::vector<FInbetweenerGrid*>& oGridArray );
        void SetCursorPosition( double iX, double iY );

    private:
        void DrawPaths( BLContext* iBLContext, FOdysseyVectorTagInbetweener* iInbetweenerTag );
        void DrawTargetGrid( BLContext* iBLContext
                           , FInbetweenerBreakdown* iBreakdown
                           , const BLRgba32& iFgColor
                           , const BLRgba32& iBgColor
                           , const BLRgba32& iHcColor );
        void UpdateBreakdown( FOdysseyVectorGroupPaint* iScene );

    private:
        UOdysseyPainterEditorVectorMatchingTool* mMatchingTool;
        ::ULIS::FRectD mChartRect;
        ::ULIS::FRectD mCursorPosition;
};
