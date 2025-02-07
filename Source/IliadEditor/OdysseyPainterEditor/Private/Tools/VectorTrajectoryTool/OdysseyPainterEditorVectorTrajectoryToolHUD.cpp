// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVector.h"
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
FOdysseyPainterEditorVectorTrajectoryToolHUD::Reset()
{
    uint64 hudFlags = mTrajectoryTool->GetEditor()->GetVectorHUDFlags();

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        UpdateSelectionInbetweenMode();
    }
}

FInbetweenerRoute*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickRoute( double iWorldX
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
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickHandle( double iWorldX
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
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickStep( double iWorldX
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

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawQuad( const FOdysseyHUDSystem::FDrawHUDParams& iParams
                                                      , const FLinearColor& iFgColor
                                                      , const FLinearColor& iBgColor
                                                      , const FLinearColor& iHcColor
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
    ::ULIS::FVec2D pointTexCoords[4] = { FOdysseyVector::MapPoint( worldMatrix, position[0] )
                                       , FOdysseyVector::MapPoint( worldMatrix, position[1] )
                                       , FOdysseyVector::MapPoint( worldMatrix, position[2] )
                                       , FOdysseyVector::MapPoint( worldMatrix, position[3] ) };
    FVector2D pointHUDCoords[4] = { iParams.mTextureToHUD.Execute( FVector2D( pointTexCoords[0].x, pointTexCoords[0].y ) )
                                  , iParams.mTextureToHUD.Execute( FVector2D( pointTexCoords[1].x, pointTexCoords[1].y ) )
                                  , iParams.mTextureToHUD.Execute( FVector2D( pointTexCoords[2].x, pointTexCoords[2].y ) )
                                  , iParams.mTextureToHUD.Execute( FVector2D( pointTexCoords[3].x, pointTexCoords[3].y ) ) };


    DrawPrimitiveLine( iParams, pointHUDCoords[0], pointHUDCoords[1], iBgColor, iHcColor, 2.0f, false );
    DrawPrimitiveLine( iParams, pointHUDCoords[1], pointHUDCoords[2], iBgColor, iHcColor, 2.0f, false );
    DrawPrimitiveLine( iParams, pointHUDCoords[2], pointHUDCoords[3], iBgColor, iHcColor, 2.0f, false );
    DrawPrimitiveLine( iParams, pointHUDCoords[3], pointHUDCoords[0], iBgColor, iHcColor, 2.0f, false );
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawTrajectory( const FOdysseyHUDSystem::FDrawHUDParams& iParams
                                                            , const FLinearColor& iFgColor
                                                            , const FLinearColor& iBgColor
                                                            , const FLinearColor& iHcColor
                                                            , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                            , FInbetweenerTrajectory* iTrajectory )
{
    FInbetweenerQuad* quad = iTrajectory->GetQuad();
    ::ULIS::FVec2D* cubicBezier = iTrajectory->GetCubicBezier();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();
    BLPoint p0TexCoords = ownerWorldMatrix.mapPoint( cubicBezier[0].x, cubicBezier[0].y );
    BLPoint p3TexCoords = ownerWorldMatrix.mapPoint( cubicBezier[3].x, cubicBezier[3].y );
    FVector2D p0HUDCoords = iParams.mTextureToHUD.Execute( FVector2D( p0TexCoords.x, p0TexCoords.y ) );
    FVector2D p3HUDCoords = iParams.mTextureToHUD.Execute( FVector2D( p3TexCoords.x, p3TexCoords.y ) );
    static FLinearColor whiteColor = FLinearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    static FLinearColor blackColor = FLinearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    static FLinearColor greenColor = FLinearColor( 0.0f, 1.0f, 0.0f, 1.0f );
    FLinearColor trajectoryColor = FLinearColor( iInbetweenerTag->GetTrajectoryColor() );

    DrawPrimitiveHandle( iParams, p0HUDCoords, FOdysseyVectorHUD::VERTEXRADIUS, iHcColor, iBgColor );
    DrawPrimitiveHandle( iParams, p3HUDCoords, FOdysseyVectorHUD::VERTEXRADIUS, iHcColor, iBgColor );

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::Linear )
    {
        DrawPrimitiveLine( iParams, p0HUDCoords, p3HUDCoords, trajectoryColor, trajectoryColor, 2.0f, false );
    }

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
    {
        BLPoint p1TexCoords = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
        BLPoint p2TexCoords = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );
        FVector2D p1HUDCoords = iParams.mTextureToHUD.Execute( FVector2D( p1TexCoords.x, p1TexCoords.y ) );
        FVector2D p2HUDCoords = iParams.mTextureToHUD.Execute( FVector2D( p2TexCoords.x, p2TexCoords.y ) );

        if( iTrajectory->GetBreakdown()->GetIndex() == 0 )
        {
            DrawQuad( iParams, iFgColor, iBgColor, iHcColor, quad );
        }

        DrawPrimitiveBezierCubic( iParams
                                , p0HUDCoords
                                , p1HUDCoords
                                , p2HUDCoords
                                , p3HUDCoords
                                , 24
                                , trajectoryColor
                                , trajectoryColor
                                , 1.0f
                                , true );

        if( mTrajectoryTool->EditionMode == eTrajectoryEditionMode::Curve )
        {
            DrawPrimitiveLine  ( iParams
                               , p0HUDCoords
                               , p1HUDCoords
                               , iTrajectory->GetStep(0)->IsAligned() ? greenColor : whiteColor
                               , blackColor
                               , 1.0f
                               , true );

            DrawPrimitiveHandle( iParams
                               , p1HUDCoords
                               , FOdysseyVectorHUD::VERTEXRADIUS
                               , iFgColor
                               , iBgColor );

            DrawPrimitiveLine  ( iParams
                               , p3HUDCoords
                               , p2HUDCoords
                               , iTrajectory->GetStep(1)->IsAligned() ? greenColor : whiteColor
                               , blackColor
                               , 1.0f
                               , true );

            DrawPrimitiveHandle( iParams
                               , p2HUDCoords
                               , FOdysseyVectorHUD::VERTEXRADIUS
                               , iFgColor
                               , iBgColor );
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
                ::ULIS::FVec2D waypointWorldCoords = FOdysseyVector::MapPoint( ownerWorldMatrix, waypointAt );
                FVector2D waypointHUDCoords = iParams.mTextureToHUD.Execute( FVector2D( waypointWorldCoords.x
                                                                                      , waypointWorldCoords.y ) );

                DrawPrimitiveVertex( iParams
                                   , waypointHUDCoords
                                   , WAYPOINTRADIUS
                                   , iFgColor
                                   , iBgColor );
            }
        }
    }
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawHUD( const FOdysseyHUDSystem::FDrawHUDParams& iParams )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    FLinearColor fgColor = FLinearColor( fg );
    FLinearColor bgColor = FLinearColor( bg );
    FLinearColor hcColor = FLinearColor( hc );
    uint64 hudFlags = mTrajectoryTool->GetEditor()->GetVectorHUDFlags();
    uint64 gridFlags = 0;

    gridFlags |= ( mTrajectoryTool->GridDisplayMode == eTrajectoryGridDisplayMode::AsPoints ) ?  FOdysseyVectorHUD::HUD_BREAKDOWN_GRID_DOTTED : 0;
    gridFlags |= ( mTrajectoryTool->EditionMode == eTrajectoryEditionMode::Add ) ? FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE_GRID : 0;

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

                DrawBreakdown( iParams
                             , breakdown
                             , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                             , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                             , ( FOdysseyVectorHUD::HUD_BREAKDOWN_SOURCE
                             | ( ( prevBreakdown == nullptr ) ? gridFlags : 0 ) ) );

                if( mTrajectoryTool->ShowInbetweens )
                {
                    DrawBreakdown( iParams
                                 , breakdown
                                 , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                                 , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                                 , FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN /*| HUD_INBETWEEN_FADEFROMTARGET*/ );

                    if( nextBreakdown )
                    {
                        DrawBreakdown( iParams
                                     , nextBreakdown
                                     , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                                     , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                                     , FOdysseyVectorHUD::HUD_BREAKDOWN_INBETWEEN /*| HUD_INBETWEEN_FADEFROMSOURCE*/ );
                    }
                }

                DrawBreakdown( iParams
                             , breakdown
                             , FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f )
                             , FLinearColor( 1.0f, 0.5f, 0.5f, 1.0f )
                             , FOdysseyVectorHUD::HUD_BREAKDOWN_TARGET );
            }

            for( FInbetweenerRoute* route : inbetweenerTag->GetRouteList() )
            {
                for( FInbetweenerTrajectory& trajectory : route->GetTrajectoryBuffer() )
                {
                    DrawTrajectory( iParams
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
                DrawQuad( iParams
                        , fgColor
                        , bgColor
                        , hcColor
                        , hoveredQuad );
            }

            // prevent a crash in case the grid is rebuilt by reset the pointer to null each time.
            mTrajectoryTool->ResetHoveredQuad();
        }
    }
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::Draw( BLContext* iBLContext )
{
}
