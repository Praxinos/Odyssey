// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"

class FOdysseyVectorTagInbetweener;
class FInbetweenerTrajectory;
class FInbetweenerRoute;
class FInbetweenerGrid;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorTrajectoryToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorTrajectoryToolHUD();
        FOdysseyPainterEditorVectorTrajectoryToolHUD(  UOdysseyPainterEditorVectorTrajectoryTool* iGridTool );

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

        FInbetweenerRoute* PickRoute( FOdysseyVectorGroupPaint* iScene
                                    , double iWorldX
                                    , double iWorldY
                                    , double iPickingRadius  );

        FInbetweenerQuad* PickSourceQuad( FInbetweenerGrid* iGrid
                                        , double iWorldX
                                        , double iworldY
                                        , double iPickingRadius );

        FInbetweenerHandleTrajectory* PickHandle( FOdysseyVectorGroupPaint* iScene
                                                , double iWorldX
                                                , double iWorldY
                                                , double iPickingRadius );

        FInbetweenerStep* PickStep( FOdysseyVectorGroupPaint* iScene
                                  , double iWorldX
                                  , double iWorldY
                                  , double iPickingRadius );

        FInbetweenerWaypoint* PickWaypoint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                          , double iWorldX
                                          , double iWorldY
                                          , double iPickingRadius );

        void SetCursorPosition( double ix, double iY );

    protected:
        static constexpr double WAYPOINTRADIUS = 3.5f;

    private:
        void DrawTrajectory( BLContext* iBLContext
                           , BLRgba32& iFgColor
                           , BLRgba32& iBgColor
                           , BLRgba32& iHcColor
                           , FOdysseyVectorTagInbetweener* iInbetweenerTag
                           , FInbetweenerTrajectory* iTrajectory );
        void DrawHoveredQuad( BLContext* iBLContext
                            , BLRgba32& iFgColor
                            , BLRgba32& iBgColor
                            , BLRgba32& iHcColor );
        void DrawSourceGrid( BLContext* iBLContext
                           , BLRgba32& iFgColor
                           , BLRgba32& iBgColor
                           , BLRgba32& iHcColor
                           , FOdysseyVectorTagInbetweener* iInbetweenerTag );

    private:
        UOdysseyPainterEditorVectorTrajectoryTool* mTrajectoryTool;
        ::ULIS::FRectD mTrajectoryRect;
        ::ULIS::FRectD mCursorPosition;
};
