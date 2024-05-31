#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"

class FOdysseyVectorTagInbetweener;
struct FInbetweenerInbetween;
class FInbetweenerGridPoint;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorMatchingToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorMatchingToolHUD();
        FOdysseyPainterEditorVectorMatchingToolHUD(  UOdysseyPainterEditorVectorMatchingTool* iGridTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        FInbetweenerInbetween* PickInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                            , double iWorldX
                                            , double iWorldY
                                            , double iRadius );
        void MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                          , FInbetweenerInbetween* iInbetween
                          , double iWorldX
                          , double iWorldY
                          , bool iRelative );
        void PickTargetPoints( FOdysseyVectorTagInbetweener* iInbetweenerTag
                             , double iWorldX
                             , double iWorldY
                             , double iRadius
                             , std::vector<FInbetweenerGridPoint*>& oPointArray
                             , std::vector<double>& oWorldDistanceArray );

    private:
        void DrawPaths( BLContext* iBLContext, FOdysseyVectorTagInbetweener* iInbetweenerTag );
        void DrawGrid( BLContext* iBLContext, FOdysseyVectorTagInbetweener* iInbetweenerTag );
        void DrawChart( BLContext* iBLContext, FOdysseyVectorTagInbetweener* iInbetweenerTag );

    private:
        UOdysseyPainterEditorVectorMatchingTool* mMatchingTool;
        ::ULIS::FRectD mChartRect;
};
