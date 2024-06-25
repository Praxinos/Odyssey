#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"

FOdysseyPainterEditorVectorBaseToolHUD::~FOdysseyPainterEditorVectorBaseToolHUD()
{
}

FOdysseyPainterEditorVectorBaseToolHUD::FOdysseyPainterEditorVectorBaseToolHUD( UOdysseyPainterEditorVectorBaseTool* iBaseTool )
{
    mBaseTool = iBaseTool;
}

void 
FOdysseyPainterEditorVectorBaseToolHUD::Load( FOdysseyVectorGroupPaint* iScene )
{
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Unload( FOdysseyVectorGroupPaint* iScene )
{

}

void
FOdysseyPainterEditorVectorBaseToolHUD::Reset( FOdysseyVectorGroupPaint* iScene )
{

}

FSelectionBox&
FOdysseyPainterEditorVectorBaseToolHUD::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxVertexMode( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    mSelectionBox.inited = false;
    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix = iScene->GetWorldMatrix();
    mSelectionBox.inverseWorldMatrix = iScene->GetInverseWorldMatrix();

    // call lambda on each object of the tree
    vectorEngine->Traverse
    ( iScene
    , 0
    , [ this
      , vectorEngine
      , iScene ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, iTraversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  ::ULIS::FRectD pathBBox;

                  if( path->GetBBoxFromSelectedVertices( pathBBox, true, true ) )
                  {
                      mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | pathBBox
                                                                : pathBBox;

                      mSelectionBox.inited = true;
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    if( mSelectionBox.inited )
    {
        ::ULIS::FRectD rect = mSelectionBox.rect;
        BLPoint p0, p1, p2, p3;

        p0 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y          );
        p1 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y          );
        p2 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y + rect.h );
        p3 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y + rect.h );

        mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                        , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                        , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                        , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxObjectMode( FOdysseyVectorGroupPaint* iScene
                                                                    , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetEngine()->GetSelectedObjectList();
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    if( ( selectedObjectList.size() <= 1 ) && ( iForceWorld == false ) )
    {
        FOdysseyVectorObject* selectedObject = selectedObjectList.size() ? selectedObjectList.front() : iScene;

        mSelectionBox.inited = true;
        mSelectionBox.rect = selectedObject->GetBBox( false );
        mSelectionBox.worldMatrix = selectedObject->GetWorldMatrix();
        mSelectionBox.inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
    }
    else 
    {
        mSelectionBox.inited = false;
        mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
        mSelectionBox.worldMatrix = iScene->GetWorldMatrix();
        mSelectionBox.inverseWorldMatrix = iScene->GetInverseWorldMatrix();

        // call lambda on each object of the tree
        vectorEngine->Traverse
        ( iScene
        , 0
        , [ this
          , iScene
          , vectorEngine
          , &selectedObjectList ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
          {
              if( vectorEngine->ObjectHasFocus( iScene, object, iTraversalFlags ) )
              {
                  ::ULIS::FRectD selectedObjectBBox = object->GetBBox( true );

                  mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | selectedObjectBBox
                                                            : selectedObjectBBox;

                  mSelectionBox.inited = true;

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }

              return 0;
          } );

        if( mSelectionBox.inited )
        {
            ::ULIS::FRectD rect = mSelectionBox.rect;
            BLPoint p0, p1, p2, p3;

            p0 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y          );
            p1 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y          );
            p2 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y + rect.h );
            p3 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y + rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                           , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxInbetweenMode( FOdysseyVectorGroupPaint* iScene
                                                                       , bool iForceWorld )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetEngine()->GetSelectedObjectList();
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    if( ( selectedObjectList.size() == 1 ) && ( iForceWorld == false ) )
    {
        FOdysseyVectorObject* selectedObject = selectedObjectList.size() ? selectedObjectList.front() : iScene;
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            mSelectionBox.inited = true;
            mSelectionBox.rect = inbetweenerTag->GetTargetBBox( false );
            mSelectionBox.worldMatrix = inbetweenerTag->GetTargetWorldMatrix();
            mSelectionBox.inverseWorldMatrix = inbetweenerTag->GetTargetInverseWorldMatrix();
        }
        else
        {
            mSelectionBox.inited = true;
            mSelectionBox.rect = selectedObject->GetBBox( false );
            mSelectionBox.worldMatrix = selectedObject->GetWorldMatrix();
            mSelectionBox.inverseWorldMatrix = selectedObject->GetInverseWorldMatrix();
        }
    }
    else 
    {
        mSelectionBox.inited = false;
        mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
        mSelectionBox.worldMatrix = iScene->GetWorldMatrix();
        mSelectionBox.inverseWorldMatrix = iScene->GetInverseWorldMatrix();

        // call lambda on each object of the tree
        vectorEngine->Traverse
        ( iScene
        , 0
        , [ this
          , iScene
          , vectorEngine
          , &selectedObjectList ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
          {
              if( vectorEngine->ObjectHasFocus( iScene, object, iTraversalFlags ) )
              {
                  FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );
                  ::ULIS::FRectD selectedObjectBBox;

                  if( tag )
                  {
                      FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                      selectedObjectBBox = inbetweenerTag->GetTargetBBox( true );
                  }
                  else
                  {
                      selectedObjectBBox = object->GetBBox( false );
                  }

                  mSelectionBox.rect = mSelectionBox.inited ? mSelectionBox.rect | selectedObjectBBox
                                                            : selectedObjectBBox;

                  mSelectionBox.inited = true;

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }

              return 0;
          } );

        if( mSelectionBox.inited )
        {
            ::ULIS::FRectD rect = mSelectionBox.rect;
            BLPoint p0, p1, p2, p3;

            p0 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y          );
            p1 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y          );
            p2 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x + rect.w, rect.y + rect.h );
            p3 = mSelectionBox.inverseWorldMatrix.mapPoint( rect.x         , rect.y + rect.h );

            mSelectionBox.rect = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                           , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                           , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );
        }
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBox( FOdysseyVectorGroupPaint* iScene
                                                          , bool iForceWorld
                                                          , uint64 iHUDFlags )
{
    if( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        //case eVectorEditionMode::Object :
        UpdateSelectionBoxObjectMode( iScene, iForceWorld );
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        //case eVectorEditionMode::Vertex:
        UpdateSelectionBoxVertexMode( iScene );
    }

    if( iHUDFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        //case eVectorEditionMode::Vertex:
        UpdateSelectionBoxInbetweenMode( iScene, iForceWorld );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawSelectionBox( BLContext* iBLContext
                                                        , FOdysseyVectorGroupPaint* iScene
                                                        , BLRgba32& iForegroundColor
                                                        , BLRgba32& iBackgroundColor
                                                        , BLRgba32& iHighlightColor
                                                        , uint64 iHUDFlags )
{
    BLRgba32 white = BLRgba32( 255, 255, 255, 255 );

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    iBLContext->save();
    iBLContext->resetMatrix();

    if( mSelectionBox.inited )
    {
        BLMatrix2D& worldMatrix = mSelectionBox.worldMatrix;
        BLPoint point[4] = { worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y                        )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x + mSelectionBox.rect.w, mSelectionBox.rect.y + mSelectionBox.rect.h )
                           , worldMatrix.mapPoint( mSelectionBox.rect.x                       , mSelectionBox.rect.y + mSelectionBox.rect.h ) };
        BLPath path;

        path.moveTo( point[0] );
        path.lineTo( point[1] );
        path.lineTo( point[2] );
        path.lineTo( point[3] );
        path.close();

        iBLContext->setStrokeStyle( iBackgroundColor );
        iBLContext->setStrokeWidth( 2.0f );
        iBLContext->strokePath( path );

        // draw box as white if nothing is selected, colored if something is selected
        iBLContext->setStrokeStyle( ( iScene->GetEngine()->GetSelectedObjectList().size() == 0 ) ? white : iForegroundColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );
    }

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawObjects( BLContext* iBLContext
                                                   , FOdysseyVectorGroupPaint* iScene
                                                   , const BLRgba32& iForegroundColor
                                                   , const BLRgba32& iBackgroundColor
                                                   , const BLRgba32& iHighlightColor
                                                   , uint64 iHUDFlags )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    iBLContext->save();
    iBLContext->resetMatrix();

    // Run lambda
    vectorEngine->Traverse
    ( iScene
    , 0
    , [ this
      , iScene
      , vectorEngine
      , &iBLContext
      , &iHUDFlags
      , &iForegroundColor
      , &iBackgroundColor
      , &iHighlightColor ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( vectorEngine->ObjectHasFocus( iScene, object, traversalFlags ) || ( iHUDFlags & HUD_DRAW_ALL ) )
          {
              if( iHUDFlags & HUD_TAGINBETWEENER_ALL )
              {
                  FOdysseyVectorTag* tag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                  if( tag )
                  {
                      FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                      FOdysseyVectorHUD::DrawInbetweens( iBLContext
                                                       , inbetweenerTag
                                                       , iForegroundColor
                                                       , iBackgroundColor
                                                       , iHighlightColor
                                                       , iHUDFlags );
                  } 
              }

              if( iHUDFlags & HUD_PATH_ALL )
              {
                  if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
                  {
                      FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                      FOdysseyVectorHUD::DrawPath( iBLContext
                                                 , path
                                                 , iForegroundColor
                                                 , iBackgroundColor
                                                 , iHighlightColor
                                                 , true
                                                 , iHUDFlags );
                  }
              }

              if( iHUDFlags & HUD_GROUPPAINT_ALL )
              {
                  if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
                  {
                      FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                      FOdysseyVectorHUD::DrawGroupPaint( iBLContext
                                                       , paintGroup
                                                       , iForegroundColor
                                                       , iBackgroundColor
                                                       , iHighlightColor
                                                       , true
                                                       , iHUDFlags );
                  }
              }

              return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
          }

          return 0;
      } );

    iBLContext->restore();
}

void
FOdysseyPainterEditorVectorBaseToolHUD::Draw( BLContext* iBLContext
                                            , FOdysseyVectorGroupPaint* iScene )
{
    FColor& fg = FOdysseyVectorHUD::GetForegroundColor();
    FColor& bg = FOdysseyVectorHUD::GetBackgroundColor();
    FColor& hc = FOdysseyVectorHUD::GetHighlightColor();
    BLRgba32 fgColor = BLRgba32( fg.R, fg.G, fg.B, fg.A );
    BLRgba32 bgColor = BLRgba32( bg.R, bg.G, bg.B, bg.A );
    BLRgba32 hcColor = BLRgba32( hc.R, hc.G, hc.B, hc.A );
    uint64 hudFlags = mBaseTool->GetEditor()->GetVectorHUDFlags();

    // Draw object details only in vertex mode
    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_PATH_VERTEX | HUD_PATH_SEGMENT );
    }

    if( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        DrawObjects( iBLContext
                   , iScene
                   , fgColor
                   , bgColor
                   , hcColor
                   , hudFlags | HUD_TAGINBETWEENER_ALL );
    }
}
