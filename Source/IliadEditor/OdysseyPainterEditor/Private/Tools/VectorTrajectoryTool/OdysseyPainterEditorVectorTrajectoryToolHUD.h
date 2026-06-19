// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>

#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"

class FOdysseyVectorTagInbetweener;
class FInbetweenerTrajectory;
class FInbetweenerRoute;
class FInbetweenerGrid;
class FInbetweenerQuad;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorTrajectoryToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        //static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorTrajectoryToolHUD();
        FOdysseyPainterEditorVectorTrajectoryToolHUD(  UOdysseyPainterEditorVectorTrajectoryTool* iGridTool );

        virtual void Load() override;
        void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        FInbetweenerInbetween* PickInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                            , double iWorldX
                                            , double iWorldY
                                            , double iRadius );
        void MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                          , FInbetweenerInbetween* iInbetween
                          , double iWorldX
                          , double iWorldY
                          , bool iRelative );

        FInbetweenerRoute* PickRoute( double iWorldX
                                    , double iWorldY
                                    , double iPickingRadius  );

        FInbetweenerQuad* PickSourceQuad( FInbetweenerGrid* iGrid
                                        , double iWorldX
                                        , double iworldY
                                        , double iPickingRadius );

        FInbetweenerHandleTrajectory* PickHandle( double iWorldX
                                                , double iWorldY
                                                , double iPickingRadius );

        FInbetweenerRoute* PickRouteFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                           , double iWorldPointX
                                           , double iWorldPointY
                                           , double iPickingRadius );

        FInbetweenerHandleTrajectory* PickHandleFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                       , double iWorldX
                                                       , double iWorldY
                                                       , double iPickingRadius );

        FInbetweenerStep* PickStep( double iWorldX
                                  , double iWorldY
                                  , double iPickingRadius );

        FInbetweenerWaypoint* PickWaypoint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                          , double iWorldX
                                          , double iWorldY
                                          , double iPickingRadius );

    protected:
        static constexpr double WAYPOINTRADIUS = 3.5f;

    private:
        void DrawTrajectory( const FOdysseyHUDElement::FDrawHUDParams& iParams
                           , const FLinearColor& iFgColor
                           , const FLinearColor& iBgColor
                           , const FLinearColor& iHcColor
                           , FOdysseyVectorTagInbetweener* iInbetweenerTag
                           , FInbetweenerTrajectory* iTrajectory );

        void DrawQuad( const FOdysseyHUDElement::FDrawHUDParams& iParams
                           , const FLinearColor& iColor
                           , FInbetweenerQuad* iQuad );

    private:
        UOdysseyPainterEditorVectorTrajectoryTool* mTrajectoryTool;
        ::ULIS::FRectD mTrajectoryRect;
};
