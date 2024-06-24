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

FInbetweenerTrajectory*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickTrajectory( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                            , double iWorldX
                                                            , double iWorldY
                                                            , double iPickingRadius )
{
    std::list<FInbetweenerTrajectory*>& trajectoryList = iInbetweenerTag->GetGrid()->GetTrajectoryList();
    BLMatrix2D& ownerWorldMatrix = iInbetweenerTag->GetOwner()->GetWorldMatrix();

    for( FInbetweenerTrajectory* trajectory : trajectoryList )
    {
        ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();

        BLPoint p0World = ownerWorldMatrix.mapPoint( cubicBezier[0].x, cubicBezier[0].y );
        BLPoint p1World = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
        BLPoint p2World = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );
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
                  FInbetweenerHandleTrajectory* trajectoryHandle = PickHandle( inbetweenerTag
                                                                             , iWorldX
                                                                             , iWorldY
                                                                             , iPickingRadius );

                  if( trajectoryHandle )
                  {
                      oTrajectoryHandleList.push_back( trajectoryHandle );
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );
}

FInbetweenerHandleTrajectory*
FOdysseyPainterEditorVectorTrajectoryToolHUD::PickHandle( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                        , double iWorldX
                                                        , double iWorldY
                                                        , double iPickingRadius )
{
    std::list<FInbetweenerTrajectory*>& trajectoryList = iInbetweenerTag->GetGrid()->GetTrajectoryList();
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
    BLPoint p1World = ownerWorldMatrix.mapPoint( cubicBezier[1].x, cubicBezier[1].y );
    BLPoint p2World = ownerWorldMatrix.mapPoint( cubicBezier[2].x, cubicBezier[2].y );
    BLPoint p3World = ownerWorldMatrix.mapPoint( cubicBezier[3].x, cubicBezier[3].y );
    BLPath path;

    iBLContext->save();
    iBLContext->resetMatrix();

    DrawCircle( iBLContext, p1World.x, p1World.y, HANDLERADIUS, iFgColor, iBgColor );

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 0, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    path.moveTo( p0World.x, p0World.y );
    path.cubicTo( p1World.x, p1World.y
                , p2World.x, p2World.y 
                , p3World.x, p3World.y );

    iBLContext->strokePath( path );

    DrawCircle( iBLContext, p2World.x, p2World.y, HANDLERADIUS, iFgColor, iBgColor );

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
                      std::list<FInbetweenerTrajectory*>& trajectoryList = inbetweenerTag->GetGrid()->GetTrajectoryList();

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