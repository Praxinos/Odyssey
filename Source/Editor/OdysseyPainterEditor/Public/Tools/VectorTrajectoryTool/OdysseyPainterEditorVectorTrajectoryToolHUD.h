#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"

class FOdysseyVectorTagInbetweener;
class FInbetweenerTrajectory;
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
        void PickTrajectory( FOdysseyVectorGroupPaint* iScene
                           , double iWorldX
                           , double iWorldY
                           , double iPickingRadius
                           , std::list<FInbetweenerTrajectory*>& oTrajectoryList );
        FInbetweenerQuad* PickSourceQuad( FInbetweenerGrid* iGrid
                                        , double iWorldX
                                        , double iworldY
                                        , double iPickingRadius );
        FInbetweenerTrajectory* PickTrajectory( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                              , double iWorldX
                                              , double iWorldY
                                              , double iPickingRadius );

        FInbetweenerHandleTrajectory* PickHandle( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                , double iWorldX
                                                , double iWorldY
                                                , double iPickingRadius );

        FInbetweenerWaypoint* PickWaypoint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                          , double iWorldX
                                          , double iWorldY
                                          , double iPickingRadius );

        void PickHandle( FOdysseyVectorGroupPaint* iScene
                       , double iWorldX
                       , double iWorldY
                       , double iPickingRadius
                       , std::list<FInbetweenerHandleTrajectory*>& oTrajectoryHandleList );

        void SetCursorPosition( double ix, double iY );

    protected:
        static constexpr double WAYPOINTRADIUS = 2.5f;

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

    private:
        UOdysseyPainterEditorVectorTrajectoryTool* mTrajectoryTool;
        ::ULIS::FRectD mTrajectoryRect;
        ::ULIS::FRectD mCursorPosition;
};
