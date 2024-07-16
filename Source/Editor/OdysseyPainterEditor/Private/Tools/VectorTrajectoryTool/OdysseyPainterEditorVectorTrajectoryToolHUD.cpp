#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryToolHUD.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "OdysseyPainterEditor.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"

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

}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickTrajectory( FOdysseyVectorGroupPaint* iScene
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iPickingRadius
                                                            , std::list<FInbetweenerTrajectory*>& oTrajectoryList )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Traverse
    ( iScene
    , 0
    , [ vectorEngine
      , iScene
      , this
      , iWorldX
      , iWorldY
      , iPickingRadius
      , &oTrajectoryList ]( FOdysseyVectorObject* object
                          , uint64 travesalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
          {
              FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

              if( tag )
              {
                  FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                  FInbetweenerTrajectory* trajectory = PickTrajectory( inbetweenerTag
                                                                     , iWorldX
                                                                     , iWorldY
                                                                     , iPickingRadius );

                  if( trajectory )
                  {
                      oTrajectoryList.push_back( trajectory );
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

FInbetweenerQuad*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickSourceQuad( FInbetweenerGrid* iGrid
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iPickingRadius )
{
    BLPoint localPick = iGrid->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );

    for( FInbetweenerQuad& quad : iGrid->GetQuadBuffer() )
    {
        if( quad.IsLinked() && quad.HitTest( localPick.x, localPick.y ) )
        {
            return &quad;
        }
    }

    return nullptr;
}

FInbetweenerTrajectory*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickTrajectory( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iPickingRadius )
{
    std::list<FInbetweenerTrajectory*>& trajectoryList = iInbetweenerTag->GetTrajectoryList();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerTrajectory* trajectory : trajectoryList )
    {
        ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
        BLPoint p0World = ownerWorldMatrix.mapPoint( cubicBezier[0].x, cubicBezier[0].y );
        BLPoint p3World = ownerWorldMatrix.mapPoint( cubicBezier[3].x, cubicBezier[3].y ); 

        if( ( ::ULIS::FVec2D( p0World.x, p0World.y )
            - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
        {
            return trajectory;
        }

        if( ( ::ULIS::FVec2D( p3World.x, p3World.y )
            - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
        {
            return trajectory;
        }
    }

    return nullptr;
}

void
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickHandle( FOdysseyVectorGroupPaint* iScene
                                                        , double iWorldX
                                                        , double iWorldY
                                                        , double iPickingRadius
                                                        , std::list<FInbetweenerHandleTrajectory*>& oTrajectoryHandleList )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Traverse
    ( iScene
    , 0
    , [ vectorEngine
      , iScene
      , this
      , iWorldX
      , iWorldY
      , iPickingRadius
      , &oTrajectoryHandleList ]( FOdysseyVectorObject* object
                                , uint64 travesalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
          {
              FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

              if( tag )
              {
                  FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                  if( inbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
                  {
                      FInbetweenerHandleTrajectory* trajectoryHandle = PickHandle( inbetweenerTag
                                                                                 , iWorldX
                                                                                 , iWorldY
                                                                                 , iPickingRadius );

                      if( trajectoryHandle )
                      {
                          oTrajectoryHandleList.push_back( trajectoryHandle );
                      }
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

FInbetweenerWaypoint*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickWaypoint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                          , double iWorldX
                                                          , double iWorldY
                                                          , double iPickingRadius )
{
    std::list<FInbetweenerTrajectory*>& trajectoryList = iInbetweenerTag->GetTrajectoryList();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
    {
        for( FInbetweenerTrajectory* trajectory : trajectoryList )
        {
            ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();

            for( FInbetweenerWaypoint& waypoint : trajectory->GetWaypointBuffer() )
            {
                ::ULIS::FVec2D waypointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( cubicBezier[0]
                                                                                               , cubicBezier[1]
                                                                                               , cubicBezier[2]
                                                                                               , cubicBezier[3]
                                                                                               , waypoint.GetT() );
                BLPoint waypointWorld = ownerWorldMatrix.mapPoint( waypointAt.x, waypointAt.y );

                if( ( ::ULIS::FVec2D( waypointWorld.x, waypointWorld.y )
                    - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
                {
                    return &waypoint;
                }
            }
        }
    }

    return nullptr;
}

FInbetweenerHandleTrajectory*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickHandle( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                        , double iWorldX
                                                        , double iWorldY
                                                        , double iPickingRadius )
{
    std::list<FInbetweenerTrajectory*>& trajectoryList = iInbetweenerTag->GetTrajectoryList();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerTrajectory* trajectory : trajectoryList )
    {
        ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();

        BLPoint p0World = ownerWorldMatrix.mapPoint( cubicBezier[0].x, cubicBezier[0].y );
        BLPoint p1World = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
        BLPoint p2World = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );
        BLPoint p3World = ownerWorldMatrix.mapPoint( cubicBezier[3].x, cubicBezier[3].y ); 


        if( ( ::ULIS::FVec2D( p1World.x, p1World.y )
            - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
        {
            return trajectory->GetHandle(0);
        }

        if( ( ::ULIS::FVec2D( p2World.x, p2World.y )
            - ::ULIS::FVec2D( iWorldX  , iWorldY   ) ).Distance() <= iPickingRadius )
        {
            return trajectory->GetHandle(1);
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
FOdysseyPainterEditorVectorTrajectoryToolHUD::DrawHoveredQuad( BLContext* iBLContext
                                                             , BLRgba32& iFgColor
                                                             , BLRgba32& iBgColor
                                                             , BLRgba32& iHcColor )
{
    FInbetweenerQuad* hoveredQuad = mTrajectoryTool->GetHoveredQuad();

    if( hoveredQuad )
    {
        ::ULIS::FRectD quadBBox = hoveredQuad->GetBBox( eInbetweenerPointPositionType::SourcePosition );
        FOdysseyVectorTagInbetweener* iInbetweenerTag = hoveredQuad->GetGrid()->GetInbetweenerTag();
/*
        BLPoint localCursor = iInbetweenerTag->GetOwner()->GetInverseWorldMatrix().mapPoint( mCursorPosition.x
                                                                                     , mCursorPosition.y );
        double u = quadBBox.w ? ( localCursor.x - quadBBox.x ) / quadBBox.w : 0.0f;
        double v = quadBBox.h ? ( localCursor.y - quadBBox.y ) / quadBBox.h : 0.0f;
        ::ULIS::FVec2D targetPosition = hoveredQuad->GetPoint( eInbetweenerPointPositionType::TargetPosition, u, v  );
        BLPoint targetWorldPosition = iInbetweenerTag->GetTargetWorldMatrix().mapPoint( targetPosition.x
                                                                                      , targetPosition.y );
        BLPath path;

        path.moveTo( mCursorPosition.x, mCursorPosition.y );
        path.lineTo( targetWorldPosition.x, targetWorldPosition.y );

        iBLContext->save();
        iBLContext->resetMatrix();

        iBLContext->setStrokeStyle( iBgColor );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );
      
        iBLContext->setStrokeStyle( iHcColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );

        iBLContext->restore();
*/
        FInbetweenerPoint** points = hoveredQuad->GetPoints();
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

        // prevent a crash in case the grid is rebuilt by reset the pointer to null each time.
        mTrajectoryTool->ResetHoveredQuad();
    }
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
    BLPath path;

    iBLContext->save();
    iBLContext->resetMatrix();

    DrawCircle( iBLContext, p0World.x, p0World.y, VERTEXRADIUS, iHcColor, iBgColor );
    DrawCircle( iBLContext, p3World.x, p3World.y, VERTEXRADIUS, iHcColor, iBgColor );

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::Linear )
    {
        path.moveTo( p0World.x, p0World.y );
        path.lineTo( p3World.x, p3World.y );

        iBLContext->setStrokeStyle( iBgColor );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        iBLContext->setStrokeStyle( iHcColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );
    }

    if( iInbetweenerTag->GetInterpolationType() == eInbetweenerInterpolationType::ARAP )
    {
        BLPoint p1World = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
        BLPoint p2World = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );

        path.moveTo( p0World.x, p0World.y );
        path.cubicTo( p1World.x, p1World.y
                    , p2World.x, p2World.y 
                    , p3World.x, p3World.y );

        iBLContext->setStrokeStyle( iBgColor );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        iBLContext->setStrokeStyle( iHcColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );

        if( mTrajectoryTool->GetPickingMode() == eTrajectoryPickingMode::Alter )
        {
            DrawLine  ( iBLContext
                      , p0World.x
                      , p0World.y
                      , p1World.x
                      , p1World.y
                      , iFgColor
                      , iBgColor );
            DrawCircle( iBLContext, p1World.x, p1World.y, VERTEXRADIUS, iFgColor, iBgColor );

            DrawLine  ( iBLContext
                      , p3World.x
                      , p3World.y
                      , p2World.x
                      , p2World.y
                      , iFgColor
                      , iBgColor );
            DrawCircle( iBLContext, p2World.x, p2World.y, VERTEXRADIUS, iFgColor, iBgColor );
        }

        for( FInbetweenerWaypoint& waypoint : iTrajectory->GetWaypointBuffer() )
        {
            ::ULIS::FVec2D waypointAt = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( cubicBezier[0]
                                                                                           , cubicBezier[1]
                                                                                           , cubicBezier[2]
                                                                                           , cubicBezier[3]
                                                                                           , waypoint.GetT() );
            BLPoint waypointWorld = ownerWorldMatrix.mapPoint( waypointAt.x, waypointAt.y );

            DrawCircle( iBLContext, waypointWorld.x, waypointWorld.y, WAYPOINTRADIUS, iFgColor, iBgColor );
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

    // Draw default
    // -> nothing in object mode.
    // -> vertices and segments in vertex mode.
    // -> inbetweens in inbetween mode.
    FOdysseyPainterEditorVectorBaseToolHUD::Draw( iBLContext, iScene );

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

        if( mTrajectoryTool->GetPickingMode() == eTrajectoryPickingMode::Add )
        {
            DrawHoveredQuad( iBLContext
                           , fgColor
                           , bgColor
                           , hcColor );
        }

        vectorEngine->Traverse
        ( iScene
        , 0
        , [ vectorEngine
          , iScene
          , iBLContext
          , &fgColor
          , &bgColor
          , &hcColor
          , this ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
          {
              if( vectorEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
              {
                  FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                  if( tag )
                  {
                      FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                      std::list<FInbetweenerTrajectory*>& trajectoryList = inbetweenerTag->GetTrajectoryList();

                      for( FInbetweenerTrajectory* trajectory : trajectoryList )
                      {
                          DrawTrajectory( iBLContext
                                        , fgColor
                                        , bgColor
                                        , hcColor
                                        , inbetweenerTag
                                        , trajectory );
                      }
                  }

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }

              return 0;
          } );
    }
}

/*
FInbetweenerInbetween*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , double iRadius )
{
    FInbetweenerTrajectory& chart = iInbetweenerTag->GetTrajectory();
    double cursorRadius = mTrajectoryRect.h *.5f;
    double cursorY = mTrajectoryRect.y + cursorRadius;

    if( mTrajectoryRect.HitTest( ::ULIS::FVec2D( iWorldX, iWorldY ) ) )
    {
        for( FInbetweenerInbetween& inbetween : chart.inbetweenBuffer )
        {
            float cursorX = mTrajectoryRect.x + ( inbetween.spacing * mTrajectoryRect.w );

            if( ( iWorldX >= ( cursorX - iRadius ) )
             && ( iWorldX <= ( cursorX + iRadius ) ) )
            {
                return &inbetween;
            }
        }
    }

    return nullptr;
}
*/

/*
void
FOdysseyPainterEditorVectorTrajectoryToolHUD::MoveInbetween( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                         , FInbetweenerInbetween* iInbetween
                                                         , double iWorldX
                                                         , double iWorldY
                                                         , bool iRelative )
{
    double newSpacing = ( iWorldX - mTrajectoryRect.x ) / mTrajectoryRect.w;

    iInbetweenerTag->MoveInbetween( iInbetween, newSpacing, iRelative );
}
*/