// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerQuad.h"
#include "InbetweenerTag/InbetweenerPoint.h"

static FInbetweenerHandleTrajectory*
PickHandleFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
                 , double iWorldX
                 , double iWorldY
                 , double iPickingRadius );
static FInbetweenerRoute*
PickRouteFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
                , double iWorldX
                , double iWorldY
                , double iPickingRadius );
static FInbetweenerStep*
PickStepFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
               , double iWorldX
               , double iWorldY
               , double iPickingRadius );

FOdysseyPainterEditorVectorTrajectoryToolHUD::~FOdysseyPainterEditorVectorTrajectoryToolHUD()
{
}

FOdysseyPainterEditorVectorTrajectoryToolHUD::FOdysseyPainterEditorVectorTrajectoryToolHUD( UOdysseyPainterEditorVectorTrajectoryTool* iTrajectoryTool )
    : FOdysseyPainterEditorVectorBaseToolHUD( iTrajectoryTool )
{
    mTrajectoryTool = iTrajectoryTool;
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{
    uint64 hudFlags = mTrajectoryTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateSelectionInbetweenMode( iScene );
    }
}

FInbetweenerRoute*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickRoute( FOdysseyVectorGroupPaint* iScene
                                                       , double iWorldX
                                                       , double iWorldY
                                                       , double iPickingRadius )
{
    for( FOdysseyVectorTagInbetweener* inbetweenerTag : mSelectedInbetweenerTagList )
    {
        FInbetweenerRoute* route = PickRouteFromTag( inbetweenerTag
                                                   , iWorldX
                                                   , iWorldY
                                                   , iPickingRadius );

        if( route )
        {
            return route;
        }
    }

    return nullptr;
}

FInbetweenerQuad*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickSourceQuad( FInbetweenerGrid* iGrid
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iPickingRadius )
{
    BLPoint localPick = iGrid->GetBreakdown()->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );

    for( FInbetweenerQuad& quad : iGrid->GetQuadBuffer() )
    {
        if( quad.IsLinked() && quad.HitTest( localPick.x, localPick.y ) )
        {
            return &quad;
        }
    }

    return nullptr;
}

static FInbetweenerRoute*
PickRouteFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
                , double iWorldPointX
                , double iWorldPointY
                , double iPickingRadius )
{
    std::list<FInbetweenerRoute*>& routeList = iInbetweenerTag->GetRouteList();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerRoute* route : routeList )
    {
        for( FInbetweenerTrajectory& trajectory : route->GetTrajectoryBuffer() )
        {
            ::ULIS::FVec2D* localBezier = trajectory.GetCubicBezier();
            ::ULIS::FVec2D worldBezier[4] = { FOdysseyVector::MapPoint( ownerWorldMatrix, localBezier[0] )
                                            , FOdysseyVector::MapPoint( ownerWorldMatrix, localBezier[1] )
                                            , FOdysseyVector::MapPoint( ownerWorldMatrix, localBezier[2] )
                                            , FOdysseyVector::MapPoint( ownerWorldMatrix, localBezier[3] ) };

            double absoluteT = FOdysseyVector::CubicBezierHitTest( ::ULIS::FVec2D( iWorldPointX
                                                                                 , iWorldPointY )
                                                                 , worldBezier[0]
                                                                 , worldBezier[1]
                                                                 , worldBezier[2]
                                                                 , worldBezier[3]
                                                                 , 16
                                                                 , iPickingRadius );
            if( absoluteT >= 0.0f )
            {
                return route;
            }
        }
    }

    return nullptr;
}

FInbetweenerHandleTrajectory*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickHandle( FOdysseyVectorGroupPaint* iScene
                                                        , double iWorldX
                                                        , double iWorldY
                                                        , double iPickingRadius )
{
    for( FOdysseyVectorTagInbetweener* inbetweenerTag : mSelectedInbetweenerTagList )
    {
        if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
        {
            FInbetweenerHandleTrajectory* trajectoryHandle = PickHandleFromTag( inbetweenerTag
                                                                              , iWorldX
                                                                              , iWorldY
                                                                              , iPickingRadius );

            if( trajectoryHandle )
            {
                return trajectoryHandle;
            }
        }
    }

    return nullptr;
}

static FInbetweenerHandleTrajectory*
PickHandleFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
                 , double iWorldX
                 , double iWorldY
                 , double iPickingRadius )
{
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerRoute* route : iInbetweenerTag->GetRouteList() )
    {
        for( FInbetweenerTrajectory& trajectory : route->GetTrajectoryBuffer() )
        {
            ::ULIS::FVec2D* cubicBezier = trajectory.GetCubicBezier();
            BLPoint p1World = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
            BLPoint p2World = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );


            if( ( ::ULIS::FVec2D( p1World.x, p1World.y )
                - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
            {
                return trajectory.GetHandle(0);
            }

            if( ( ::ULIS::FVec2D( p2World.x, p2World.y )
                - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
            {
                return trajectory.GetHandle(1);
            }
        }
    }

    return nullptr;
}

FInbetweenerStep*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickStep( FOdysseyVectorGroupPaint* iScene
                                                      , double iWorldX
                                                      , double iWorldY
                                                      , double iPickingRadius )
{
    FInbetweenerStep* retStep = nullptr;

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : mSelectedInbetweenerTagList )
    {
        if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
        {
            FInbetweenerStep* step = PickStepFromTag( inbetweenerTag
                                                    , iWorldX
                                                    , iWorldY
                                                    , iPickingRadius );

            if( step )
            {
                return step;
            }
        }
    }

    return retStep;
}

static FInbetweenerStep*
PickStepFromTag( FOdysseyVectorTagInbetweener* iInbetweenerTag
               , double iWorldX
               , double iWorldY
               , double iPickingRadius )
{
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerRoute* route : iInbetweenerTag->GetRouteList() )
    {
        for( FInbetweenerTrajectory& trajectory : route->GetTrajectoryBuffer() )
        {
            ::ULIS::FVec2D* cubicBezier = trajectory.GetCubicBezier();
            BLPoint p0World = ownerWorldMatrix.mapPoint( cubicBezier[0].x, cubicBezier[0].y );
            BLPoint p3World = ownerWorldMatrix.mapPoint( cubicBezier[3].x, cubicBezier[3].y );


            if( ( ::ULIS::FVec2D( p0World.x, p0World.y )
                - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
            {
                return trajectory.GetStep(0);
            }

            if( ( ::ULIS::FVec2D( p3World.x, p3World.y )
                - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
            {
                return trajectory.GetStep(1);
            }
        }
    }

    return nullptr;
}

FInbetweenerWaypoint*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickWaypoint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                          , double iWorldX
                                                          , double iWorldY
                                                          , double iPickingRadius )
{
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerRoute* route : iInbetweenerTag->GetRouteList() )
    {
        if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
        {
            for( FInbetweenerTrajectory& trajectory : route->GetTrajectoryBuffer() )
            {
                ::ULIS::FVec2D* cubicBezier = trajectory.GetCubicBezier();

                for( uint32 i = 1; i < trajectory.GetWaypointBuffer().size() - 1; i++ )
                {
                    FInbetweenerWaypoint& waypoint = trajectory.GetWaypointBuffer()[i];
                    // spacing is linear, unlike cubic t value. We have to find the correct t value for a given spacing
                    double cubicT = trajectory.GetCubicT( waypoint.GetT() );
                    ::ULIS::FVec2D waypointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( cubicBezier[0]
                                                                                                   , cubicBezier[1]
                                                                                                   , cubicBezier[2]
                                                                                                   , cubicBezier[3]
                                                                                                   , cubicT );
                    BLPoint waypointWorld = ownerWorldMatrix.mapPoint( waypointAt.x, waypointAt.y );

                    if( ( ::ULIS::FVec2D( waypointWorld.x, waypointWorld.y )
                        - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
                    {
                        return &waypoint;
                    }
                }
            }
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::SetCursorPosition( double iX, double iY )
{
    mCursorPosition.x = iX;
    mCursorPosition.y = iY;
}

/*
void
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawSourceGrid( BLContext* iBLContext
                                                            , BLRgba32& iFgColor
                                                            , BLRgba32& iBgColor
                                                            , BLRgba32& iHcColor
                                                            , FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    FInbetweenerBreakdown* firstBreakdown = iInbetweenerTag->GetBreakdownList().front();
    BLMatrix2D& worldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();
    BLRgba32 greyColor = BLRgba32( 120, 120, 120, 255 );

    iBLContext->save();
    iBLContext->resetMatrix();

    for( FInbetweenerQuad& quad : firstBreakdown->GetGrid()->GetQuadBuffer() )
    {
        FInbetweenerPoint** points = quad.GetPoints();
        ::ULIS::FVec2D position[4] = { points[0]->GetSourcePosition()
                                     , points[1]->GetSourcePosition()
                                     , points[2]->GetSourcePosition()
                                     , points[3]->GetSourcePosition() };
        BLPoint pt[4] = { worldMatrix.mapPoint( position[0].x, position[0].y )
                        , worldMatrix.mapPoint( position[1].x, position[1].y )
                        , worldMatrix.mapPoint( position[2].x, position[2].y )
                        , worldMatrix.mapPoint( position[3].x, position[3].y ) };
        BLPath path;

        path.moveTo( pt[0].x, pt[0].y );
        path.lineTo( pt[1].x, pt[1].y );
        path.lineTo( pt[2].x, pt[2].y );
        path.lineTo( pt[3].x, pt[3].y );
        path.close();

        iBLContext->setStrokeStyle( greyColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );
    }

    iBLContext->restore();
}
*/

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawQuad( BLContext* iBLContext
                                                      , BLRgba32& iFgColor
                                                      , BLRgba32& iBgColor
                                                      , BLRgba32& iHcColor
                                                      , FInbetweenerQuad* iQuad )
{
    ::ULIS::FRectD quadBBox = iQuad->GetBBox( eInbetweenerPointPositionType::SourcePosition );
    FOdysseyVectorTagInbetweener* iInbetweenerTag = iQuad->GetGrid()->GetBreakdown()->GetInbetweenerTag();
    FInbetweenerPoint** points = iQuad->GetPoints();
    ::ULIS::FVec2D position[4] = { points[0]->GetSourcePosition()
                                 , points[1]->GetSourcePosition()
                                 , points[2]->GetSourcePosition()
                                 , points[3]->GetSourcePosition() };
    BLMatrix2D& worldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();
    BLPoint pt[4] = { worldMatrix.mapPoint( position[0].x, position[0].y )
                    , worldMatrix.mapPoint( position[1].x, position[1].y )
                    , worldMatrix.mapPoint( position[2].x, position[2].y )
                    , worldMatrix.mapPoint( position[3].x, position[3].y ) };
    BLPath path;

    path.moveTo( pt[0].x, pt[0].y );
    path.lineTo( pt[1].x, pt[1].y );
    path.lineTo( pt[2].x, pt[2].y );
    path.lineTo( pt[3].x, pt[3].y );
    path.close();

    iBLContext->save();
    iBLContext->resetMatrix();

    iBLContext->setStrokeStyle( iBgColor );
    iBLContext->setStrokeWidth( 2.0f );
    iBLContext->strokePath( path );

    iBLContext->setStrokeStyle( iHcColor );
    iBLContext->setStrokeWidth( 1.0f );
    iBLContext->strokePath( path );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawTrajectory( BLContext* iBLContext
                                                            , BLRgba32& iFgColor
                                                            , BLRgba32& iBgColor
                                                            , BLRgba32& iHcColor
                                                            , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                            , FInbetweenerTrajectory* iTrajectory )
{
    FInbetweenerQuad* quad = iTrajectory->GetQuad();
    ::ULIS::FVec2D* cubicBezier = iTrajectory->GetCubicBezier();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();
    BLPoint p0World = ownerWorldMatrix.mapPoint( cubicBezier[0].x, cubicBezier[0].y );
    BLPoint p3World = ownerWorldMatrix.mapPoint( cubicBezier[3].x, cubicBezier[3].y );
    static BLRgba32 whiteColor = BLRgba32( 0xFF, 0xFF, 0xFF, 0xFF );
    static BLRgba32 blackColor = BLRgba32( 0x00, 0x00, 0x00, 0xFF );
    static BLRgba32 greenColor = BLRgba32( 0x00, 0xFF, 0x00, 0xFF );
    FColor trajectoryColor = iInbetweenerTag->GetTrajectoryColor();
    BLPath path;

    iBLContext->save();
    iBLContext->resetMatrix();

    DrawCircle( iBLContext, p0World.x, p0World.y, VERTEXRADIUS, iHcColor, iBgColor );
    DrawCircle( iBLContext, p3World.x, p3World.y, VERTEXRADIUS, iHcColor, iBgColor );

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::Linear )
    {
        path.moveTo( p0World.x, p0World.y );
        path.lineTo( p3World.x, p3World.y );

        iBLContext->setStrokeStyle( BLRgba32( trajectoryColor.R
                                            , trajectoryColor.G
                                            , trajectoryColor.B
                                            , trajectoryColor.A ) );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        /*iBLContext->setStrokeStyle( iHcColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );*/
    }

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
    {
        BLPoint p1World = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
        BLPoint p2World = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );

        if( iTrajectory->GetBreakdown()->GetIndex() == 0 )
        {
            DrawQuad( iBLContext, iFgColor, iBgColor, iHcColor, quad );
        }

        path.moveTo( p0World.x, p0World.y );
        path.cubicTo( p1World.x, p1World.y
                    , p2World.x, p2World.y
                    , p3World.x, p3World.y );

        iBLContext->setStrokeStyle( BLRgba32( trajectoryColor.R
                                            , trajectoryColor.G
                                            , trajectoryColor.B
                                            , trajectoryColor.A ) );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        /*iBLContext->setStrokeStyle( iHcColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );*/

        if( mTrajectoryTool->EditionMode == eTrajectoryEditionMode::Curve )
        {
            DrawLine  ( iBLContext
                      , p0World.x
                      , p0World.y
                      , p1World.x
                      , p1World.y
                      , iTrajectory->GetStep(0)->IsAligned() ? greenColor : whiteColor
                      , blackColor );
            DrawCircle( iBLContext, p1World.x, p1World.y, VERTEXRADIUS, iFgColor, iBgColor );

            DrawLine  ( iBLContext
                      , p3World.x
                      , p3World.y
                      , p2World.x
                      , p2World.y
                      , iTrajectory->GetStep(1)->IsAligned() ? greenColor : whiteColor
                      , blackColor );
            DrawCircle( iBLContext, p2World.x, p2World.y, VERTEXRADIUS, iFgColor, iBgColor );
        }

        if( mTrajectoryTool->EditionMode == eTrajectoryEditionMode::Spacing )
        {
            for( uint32 i = 1; i < iTrajectory->GetWaypointBuffer().size() - 1; i++ )
            {
                FInbetweenerWaypoint& waypoint = iTrajectory->GetWaypointBuffer()[i];
                // spacing is linear, unlike cubic t value. We have to find the correct t value for a given spacing
                double cubicT = iTrajectory->GetCubicT( waypoint.GetT() );
                ::ULIS::FVec2D waypointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( cubicBezier[0]
                                                                                               , cubicBezier[1]
                                                                                               , cubicBezier[2]
                                                                                               , cubicBezier[3]
                                                                                               , cubicT );
                BLPoint waypointWorld = ownerWorldMatrix.mapPoint( waypointAt.x, waypointAt.y );

                DrawCircle( iBLContext, waypointWorld.x, waypointWorld.y, WAYPOINTRADIUS, iFgColor, iBgColor );
            }
        }
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::Draw( BLContext* iBLContext
                                                  , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mTrajectoryTool->GetEditor()->GetVectorHUDFlags();
    uint64 gridFlags = 0;

    gridFlags |= ( mTrajectoryTool->GridDisplayMode == eTrajectoryGridDisplayMode::AsPoints ) ?  HUD_BREAKDOWN_GRID_DOTTED : 0;
    gridFlags |= ( mTrajectoryTool->EditionMode == eTrajectoryEditionMode::Add ) ? HUD_BREAKDOWN_SOURCE_GRID : 0;

    iBLContext->save();
    // do not add-up colors
    iBLContext->setCompOp( BL_COMP_OP_SRC_COPY );

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    //FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        for( FOdysseyVectorTagInbetweener* inbetweenerTag : mSelectedInbetweenerTagList )
        {
            inbetweenerTag->LockDrawing();

            for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
            {
                FInbetweenerBreakdown* nextBreakdown = breakdown->GetNextBreakdown();
                FInbetweenerBreakdown* prevBreakdown = breakdown->GetPrevBreakdown();

                FOdysseyVectorHUD::DrawBreakdown( iScene
                                                , iBLContext
                                                , breakdown
                                                , BLRgba32( 127, 127, 127, 255 )
                                                , BLRgba32( 255, 127, 127, 255 )
                                                , ( HUD_BREAKDOWN_SOURCE
                                                  | ( ( prevBreakdown == nullptr ) ? gridFlags : 0 ) ) );

                if( mTrajectoryTool->ShowInbetweens )
                {
                    FOdysseyVectorHUD::DrawBreakdown( iScene
                                                    , iBLContext
                                                    , breakdown
                                                    , BLRgba32( 127, 127, 127, 255 )
                                                    , BLRgba32( 255, 127, 127, 255 )
                                                    , HUD_BREAKDOWN_INBETWEEN /*| HUD_INBETWEEN_FADEFROMTARGET*/ );

                    if( nextBreakdown )
                    {
                        FOdysseyVectorHUD::DrawBreakdown( iScene
                                                        , iBLContext
                                                        , nextBreakdown
                                                        , BLRgba32( 127, 127, 127, 255 )
                                                        , BLRgba32( 255, 127, 127, 255 )
                                                        , HUD_BREAKDOWN_INBETWEEN /*| HUD_INBETWEEN_FADEFROMSOURCE*/ );
                    }
                }

                FOdysseyVectorHUD::DrawBreakdown( iScene
                                                , iBLContext
                                                , breakdown
                                                , BLRgba32( 127, 127, 127, 255 )
                                                , BLRgba32( 255, 127, 127, 255 )
                                                , HUD_BREAKDOWN_TARGET );
            }

            for( FInbetweenerRoute* route : inbetweenerTag->GetRouteList() )
            {
                for( FInbetweenerTrajectory& trajectory : route->GetTrajectoryBuffer() )
                {
                    DrawTrajectory( iBLContext
                                  , fgColor
                                  , bgColor
                                  , hcColor
                                  , inbetweenerTag
                                  , &trajectory );
                }
            }

            inbetweenerTag->UnlockDrawing();
        }

        if( mTrajectoryTool->EditionMode == eTrajectoryEditionMode::Add )
        {
            FInbetweenerQuad* hoveredQuad = mTrajectoryTool->GetHoveredQuad();

            if( hoveredQuad )
            {
                DrawQuad( iBLContext
                        , fgColor
                        , bgColor
                        , hcColor
                        , hoveredQuad );
            }

            // prevent a crash in case the grid is rebuilt by reset the pointer to null each time.
            mTrajectoryTool->ResetHoveredQuad();
        }
    }

    iBLContext->restore();
}
