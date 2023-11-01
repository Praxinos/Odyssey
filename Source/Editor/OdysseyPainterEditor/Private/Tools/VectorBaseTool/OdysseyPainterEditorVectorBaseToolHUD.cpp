#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"

FOdysseyPainterEditorVectorBaseToolHUD::~FOdysseyPainterEditorVectorBaseToolHUD()
{
}

FOdysseyPainterEditorVectorBaseToolHUD::FOdysseyPainterEditorVectorBaseToolHUD( UOdysseyPainterEditorVectorBaseTool* iBaseTool )
{
    mBaseTool = iBaseTool;
}
/*
void
FOdysseyPainterEditorVectorBaseToolHUD::GetAlteredObjectList( FOdysseyVectorScene* iScene
                                                            , std::list<FOdysseyVectorObject*>& oObjectList )
{
    FOdysseyVectorEngine::Traverse
    ( iScene
    , iScene
    , mBaseTool->GetEditor()->GetVectorEditionFlags()
    , [ &oObjectList ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
        {
            oObjectList.push_back( object);

            return false; // keep traversing
        } );
}
*/

// tells in which case an object is displayed by the tool
bool
FOdysseyPainterEditorVectorBaseToolHUD::IsObjectDisplayed( FOdysseyVectorScene* iScene
                                                         , FOdysseyVectorObject* iObject
                                                         , uint64 iHUDFlags
                                                         , uint64 iTraversalFlags )
{
    if( iScene->GetSelectedObjectList().size() == 0 )
    {
        return true;
    }
    else
    {
        if( iObject->IsSelected() || ( iTraversalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_ACCEPTED ) )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawObjects( BLContext* iBLContext
                                                   , FOdysseyVectorScene* iScene
                                                   , const BLRgba32& iForegroundColor
                                                   , const BLRgba32& iBackgroundColor
                                                   , const BLRgba32& iHighlightColor
                                                   , uint64 iHUDFlags )
{
    iBLContext->save();
    iBLContext->resetMatrix();

    // Run lambda
    FOdysseyVectorEngine::Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , &iBLContext
      , &iScene
      , &iHUDFlags
      , &iForegroundColor
      , &iBackgroundColor
      , &iHighlightColor ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( mBaseTool->DisplayObjectHUD( iScene, object, iHUDFlags, traversalFlags ) )
          {
              if( iHUDFlags & VIEW_PATH_ALL )
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

              if( iHUDFlags & VIEW_GROUPPAINT_ALL )
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

FSelectionBox&
FOdysseyPainterEditorVectorBaseToolHUD::GetSelectionBox()
{
    return mSelectionBox;
}

void
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxVertexMode( FOdysseyVectorScene* iScene
                                                                    , uint64 iHUDFlags )
{
    mSelectionBox.inited = false;
    mSelectionBox.rect = ::ULIS::FRectD( 0, 0, 0, 0 );
    mSelectionBox.worldMatrix = iScene->GetWorldMatrix();
    mSelectionBox.inverseWorldMatrix = iScene->GetInverseWorldMatrix();

    // call lambda on each object of the tree
    FOdysseyVectorEngine::Traverse
    ( iScene
    , iScene
    , 0
    , [ this
      , iScene
      , &iHUDFlags ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
      {
          if( mBaseTool->DisplayObjectHUD( iScene, object, iHUDFlags, iTraversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);
                  ::ULIS::FRectD pathBBox;

                  if( path->GetBBoxFromSelectedVertices( pathBBox, true ) )
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
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBoxObjectMode( FOdysseyVectorScene* iScene
                                                                    , bool iForceWorld
                                                                    , uint64 iHUDFlags )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    if( ( selectedObjectList.size() == 1 ) && ( iForceWorld == false ) )
    {
        FOdysseyVectorObject* selectedObject = selectedObjectList.front();

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
        FOdysseyVectorEngine::Traverse
        ( iScene
        , iScene
        , 0
        , [ this
          , iScene
          , &iHUDFlags
          , &selectedObjectList ]( FOdysseyVectorObject* object, uint64 iTraversalFlags ) -> uint64
          {
              if( mBaseTool->DisplayObjectHUD( iScene, object, iHUDFlags, iTraversalFlags ) )
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
FOdysseyPainterEditorVectorBaseToolHUD::UpdateSelectionBox( FOdysseyVectorScene* iScene
                                                          , bool iForceWorld
                                                          , uint64 iHUDFlags )
{
    if( iHUDFlags & VIEW_MODE_OBJECT )
    {
        //case eVectorEditionMode::Object :
        UpdateSelectionBoxObjectMode( iScene, iForceWorld, iHUDFlags );
    }

    if( iHUDFlags & VIEW_MODE_VERTEX )
    {
        //case eVectorEditionMode::Vertex:
        UpdateSelectionBoxVertexMode( iScene, iHUDFlags );
    }
}

void
FOdysseyPainterEditorVectorBaseToolHUD::DrawSelectionBox( BLContext* iBLContext
                                                        , FOdysseyVectorScene* iScene
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
        iBLContext->setStrokeStyle( ( iScene->GetSelectedObjectList().size() == 0 ) ? white : iForegroundColor );
        iBLContext->setStrokeWidth( 1.0f );
        iBLContext->strokePath( path );
    }

    iBLContext->restore();
}
