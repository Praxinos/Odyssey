// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorTransformTool::~UOdysseyPainterEditorVectorTransformTool()
{
}

UOdysseyPainterEditorVectorTransformTool::UOdysseyPainterEditorVectorTransformTool()
    : UOdysseyPainterEditorVectorSelectionTool( new FOdysseyPainterEditorVectorTransformToolHUD( this ) )
    , mPickedPivot( nullptr )
    , mDragging( false )
    , PickingRadius(10.0f)
    , Uniform( true )
    , World( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.TransformTool32");

    mTransformHUD = static_cast<FOdysseyPainterEditorVectorTransformToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorTransformTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorTransformTool::UnloadVector( FOdysseyVectorGroupPaint* iScene )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorTransformTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    mTransformHUD->CenterGizmo();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorTransformTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FKey& iKey )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
    }

    return UOdysseyPainterEditorVectorSelectionTool::OnKeyDownVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorTransformTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FKey& iKey )
{
    Uniform = UniformAtKeyDown;

    return UOdysseyPainterEditorVectorSelectionTool::OnKeyUpVector( iScene, iKey );
}

uint64
UOdysseyPainterEditorVectorTransformTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    if( mDragging == false )
    {
        uint32 width = iEngine->GetPreferredWidth();
        uint32 height = iEngine->GetPreferredHeight();

        imageRegion.x = 0;
        imageRegion.y = 0;
        imageRegion.w = width;
        imageRegion.h = height;

        if( mTransformHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) )
        {
            redrawRegion = imageRegion; // redraw the whole image when the gizmo must be redrawn
        }
    }


    if( redrawRegion.Area() )
    {
        iEngine->GetInvalidTileMap().Invalidate(redrawRegion);

        return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
    }

    return 0;
}

void
UOdysseyPainterEditorVectorTransformTool::GetTransformedObjectList( FOdysseyVectorGroupPaint* iScene
                                                                  , std::list<FOdysseyVectorObject*>& oObjectList )
{
    FOdysseyVectorEngine* vectorEngine = iScene->GetEngine();

    vectorEngine->Traverse
    ( iScene
    , iScene
    , 0
    , [ iScene
      , vectorEngine
      , &oObjectList ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          // transform is recursive per se, do not recurse if the parent was transformed already
          if( ( travesalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS ) == 0 )
          {
              if( vectorEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
              {
                  oObjectList.push_back( object );

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }

          return 0;
      } );
}

uint64
UOdysseyPainterEditorVectorTransformTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    mTransformHUD->SetCenterGizmo( false );

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
        uint32 hudFlags = mTransformHUD->GetFlags();

        mDragging = false;
        mScreenMouseAtDown = FSlateApplication::Get().GetCursorPos();

        mPickedPivot = hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ZAXIS ? &mTransformHUD->GetGizmo() : nullptr;

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            mTransformedVertexArray.clear();
            mTransformedHandleArray.clear();

            GetSelectedVertices( iScene, mTransformedVertexArray );
            // static call
            UOdysseyPainterEditorVectorBaseTool::GetSegmentHandlesFromVertices( mTransformedVertexArray
                                                                              , mTransformedHandleArray );

            // remember for undos. we don't register the undo in the mouse down event yet because
            // it could conflict with the undo created by the mouse up event in the case of a no-drag
            mUndo = new FOdysseyVectorUndoPointPosition( iScene
                                                       , mTransformedVertexArray
                                                       , mTransformedHandleArray );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            std::list<FOdysseyVectorObject*> transformedObjectList;

            GetTransformedObjectList( iScene, transformedObjectList );

            // remember for undos. we don't register the undo in the mouse down event yet because
            // it could conflict with the undo created by th emouse up event in the case of a no-drag
            mUndo = new FOdysseyVectorUndoObjectTransform( iScene, transformedObjectList );
        }

        if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
        {
            mTransformHUD->ShowSelectionBox( false );
        }
    }
 
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

static void
TransformPoint( FOdysseyVectorPoint* iPoint
              , BLMatrix2D& iSpaceMatrix
              , BLMatrix2D& iInverseSpaceMatrix
              , BLMatrix2D& iTransformationMatrix )
{
    FOdysseyVectorObject* ownerObject = nullptr;
    uint32 pointClass = iPoint->GetClass();

    if( pointClass == FOdysseyVectorVertex::StaticClass() )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(iPoint);

        ownerObject = vertex->GetOwner();
    }

    if( pointClass == FOdysseyVectorHandleSegment::StaticClass() )
    {
        FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>(iPoint);

        ownerObject = segmentHandle->GetOwner()->GetOwnerAsPath();
    }

    if( pointClass == FOdysseyVectorBucket::StaticClass() )
    {
        FOdysseyVectorBucket* bucket = static_cast<FOdysseyVectorBucket*>(iPoint);

        ownerObject = bucket->GetOwner();
    }

    if( ownerObject )
    {
        ::ULIS::FVec2D& localCoords = iPoint->GetCoords();
        BLPoint worldCoords = ownerObject->GetWorldMatrix().mapPoint( localCoords.x, localCoords.y );
        BLPoint spaceCoords = iInverseSpaceMatrix.mapPoint( worldCoords.x, worldCoords.y );
        BLPoint transCoords = iTransformationMatrix.mapPoint( spaceCoords.x, spaceCoords.y );
        BLPoint newWorldCoords = iSpaceMatrix.mapPoint( transCoords.x, transCoords.y );
        BLPoint newLocalCoords = ownerObject->GetInverseWorldMatrix().mapPoint( newWorldCoords.x, newWorldCoords.y );

        iPoint->Set( newLocalCoords.x, newLocalCoords.y );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::TranslateObjectSelection( FOdysseyVectorEngine* iEngine
                                                                  , FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    uint32 hudFlags = mTransformHUD->GetFlags();
    ::ULIS::FVec2D& pivot = mTransformHUD->GetGizmo();
    BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
    BLMatrix2D inverseSpaceMatrix;
    BLMatrix2D translateMatrix;
    BLPoint translateBy(0,0);
    BLPoint localDelta;

    BLMatrix2D::invert( inverseSpaceMatrix, spaceMatrix );

    localDelta = inverseSpaceMatrix.mapVector( iPointInTexture.deltaPosition.X
                                             , iPointInTexture.deltaPosition.Y );

    translateMatrix.reset();

    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS     )
     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
    {
        translateBy.x = localDelta.x;

        translateMatrix.translate( translateBy.x, 0 );
    }

    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS     )
     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
    {
        translateBy.y = localDelta.y;

        translateMatrix.translate( 0, translateBy.y );
    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        for( int i = 0; i < mTransformedVertexArray.size(); i++ )
        {
            TransformPoint( mTransformedVertexArray[i]
                          , spaceMatrix
                          , inverseSpaceMatrix
                          , translateMatrix );
        }

        for( int i = 0; i < mTransformedHandleArray.size(); i++ )
        {
            TransformPoint( mTransformedHandleArray[i]
                          , spaceMatrix
                          , inverseSpaceMatrix
                          , translateMatrix );
        }

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );

        // update the selection box with the newly modified matrices
        iEngine->ResetHUD();

        // replace pivot correctly.
        pivot.x += translateBy.x;
        pivot.y += translateBy.y;
    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        BLPoint spacePivot = BLPoint( pivot.x - selectionBox.rect.x
                                    , pivot.y - selectionBox.rect.y );

        // run lambda recursively on altered objects
        iEngine->Traverse
        ( iScene
        , iScene
        , 0
        , [ iScene
          , iEngine
          , &spaceMatrix
          , &inverseSpaceMatrix
          , &translateMatrix ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
          {
              // transform is recursive per se, do not recurse if the parent was transformed already
              if( ( travesalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS ) == 0 )
              {
                  if( iEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
                  {
                      double translationX;
                      double translationY;
                      double rotation;
                      double scalingX;
                      double scalingY;
                      BLMatrix2D objectSpaceMatrix;
                      BLMatrix2D objectTranslateMatrix;
                      BLMatrix2D objectLocalMatrix;
                      BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();
                      BLMatrix2D parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();

                      // transfer object in "Selection Space" coordinates system
                      FOdysseyVector::MatrixMultiply( inverseSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                      // translate the object (local to the "Selection Space" coordinates system)
                      FOdysseyVector::MatrixMultiply( translateMatrix, objectSpaceMatrix, objectTranslateMatrix );

                      // transfer the object back to world coordinates system
                      FOdysseyVector::MatrixMultiply( spaceMatrix, objectTranslateMatrix, objectWorldMatrix );

                      // Convert the object to its parent coordinate system, i.e its local coordinates system.
                      FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

                      // Extract the local transformations
                      FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                            , &translationX
                                                            , &translationY
                                                            , &rotation // in radians
                                                            , &scalingX
                                                            , &scalingY );

                      // Apply the local transformations
                      object->Translate( translationX, translationY );
                      object->Rotate( rotation / M_PI * 180 ); // in degrees
                      object->Scale( scalingX, scalingY );

                      object->UpdateMatrix();

                      return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
                  }
              }

              return 0;
          } );

        // Update the matrix for all objects
        //iScene->UpdateMatrix();

        iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );

        // update the selection box with the newly modified matrices
        iEngine->ResetHUD();

        // replace pivot correctly.
        pivot.x = selectionBox.rect.x + spacePivot.x;
        pivot.y = selectionBox.rect.y + spacePivot.y;
    }
}

double
UOdysseyPainterEditorVectorTransformTool::GetRotationAngle( const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    BLPoint pt[2] = { selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                                              , iPointInTexture.y - iPointInTexture.deltaPosition.Y )
                    , selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x
                                                              , iPointInTexture.y ) };
    ::ULIS::FVec2D& pivot = mTransformHUD->GetGizmo();
    ::ULIS::FVec2D vector[2];
    double angle = 0.0f;

    vector[0].x = pt[0].x - pivot.x;
    vector[0].y = pt[0].y - pivot.y;

    vector[1].x = pt[1].x - pivot.x;
    vector[1].y = pt[1].y - pivot.y;

    if( vector[0].DistanceSquared() )
    {
        vector[0].Normalize();
    }

    if( vector[1].DistanceSquared() )
    {
        vector[1].Normalize();
    }

    angle = fabs( acos( vector[0].DotProduct( vector[1] ) ) );

    return FOdysseyVector::Cross2D( vector[0], vector[1] ) > 0.0f ? angle : - angle;
}

void
UOdysseyPainterEditorVectorTransformTool::RotateObjectSelection( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorGroupPaint* iScene
                                                               , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    BLMatrix2D spaceMatrix/* = selectionBox.worldMatrix*/;
    BLMatrix2D inverseSpaceMatrix;
    ::ULIS::FVec2D& pivot = mTransformHUD->GetGizmo();
    BLMatrix2D rotateMatrix;
    double rotationAngle = GetRotationAngle( iPointInTexture );
    BLPoint worldPivot = selectionBox.worldMatrix.mapPoint( pivot.x, pivot.y );

    rotateMatrix.resetToRotation( rotationAngle ); // Radians

    spaceMatrix.reset();
    spaceMatrix.translate( worldPivot.x, worldPivot.y );

    BLMatrix2D::invert( inverseSpaceMatrix, spaceMatrix );

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        for( int i = 0; i < mTransformedVertexArray.size(); i++ )
        {
            TransformPoint( mTransformedVertexArray[i]
                          , spaceMatrix
                          , inverseSpaceMatrix
                          , rotateMatrix );
        }

        for( int i = 0; i < mTransformedHandleArray.size(); i++ )
        {
            TransformPoint( mTransformedHandleArray[i]
                          , spaceMatrix
                          , inverseSpaceMatrix
                          , rotateMatrix );
        }
    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        // run lambda recursively on altered objects
        iEngine->Traverse
        ( iScene
        , iScene
        , 0
        ,[ iScene
          , iEngine
          , &spaceMatrix
          , &inverseSpaceMatrix
          , &rotateMatrix ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
          {
              // transform is recursive per se, do not recurse if the parent was transformed already
              if( ( travesalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS ) == 0 )
              {
                  if( iEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
                  {
                      double translationX;
                      double translationY;
                      double rotation;
                      double scalingX;
                      double scalingY;
                      BLMatrix2D objectSpaceMatrix;
                      BLMatrix2D objectScaledMatrix;
                      BLMatrix2D objectLocalMatrix;
                      BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();
                      BLMatrix2D parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();

                      // transfer object in "Rotation Space" coordinates system
                      FOdysseyVector::MatrixMultiply( inverseSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                      // rotate the object (local to the "Rotation Space" coordinates system)
                      FOdysseyVector::MatrixMultiply( rotateMatrix, objectSpaceMatrix, objectScaledMatrix );

                      // transfer the object back to world coordinates system
                      FOdysseyVector::MatrixMultiply( spaceMatrix, objectScaledMatrix, objectWorldMatrix );

                      // Convert the object to its parent coordinate system, i.e its local coordinates system.
                      FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

                      // Extract the local transformations
                      FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                            , &translationX
                                                            , &translationY
                                                            , &rotation // in radians
                                                            , &scalingX
                                                            , &scalingY );

                      // Apply the local transformations
                      object->Translate( translationX, translationY );
                      object->Rotate( rotation / M_PI * 180 ); // in degrees
                      object->Scale( scalingX, scalingY );

                      object->UpdateMatrix();

                      return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
                  }
              }

              return 0;
          } );
    }

    // Update the matrix for all objects
    //iScene->UpdateMatrix();

    iScene->Update( FOdysseyVectorObject::KEEPINVALIDATED );

    // update the selection box with the newly modified matrices
    iEngine->ResetHUD();

    // replace pivot correctly.
    BLPoint spacePivot = selectionBox.inverseWorldMatrix.mapPoint( worldPivot.x, worldPivot.y );
    pivot.x = spacePivot.x;
    pivot.y = spacePivot.y;
}

void
UOdysseyPainterEditorVectorTransformTool::ScaleObjectSelection( FOdysseyVectorEngine* iEngine
                                                              , FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    uint32 hudFlags = mTransformHUD->GetFlags();
    BLPoint oldLocalCoords = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x - iPointInTexture.deltaPosition.X
                                                                     , iPointInTexture.y - iPointInTexture.deltaPosition.Y );
    BLPoint localCoords = selectionBox.inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );
    double difx = localCoords.x - oldLocalCoords.x;
    double dify = localCoords.y - oldLocalCoords.y;
    double oldX1 = selectionBox.rect.x
            , oldY1 = selectionBox.rect.y
            , oldX2 = selectionBox.rect.x + selectionBox.rect.w
            , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
    double oldDiagonal = sqrt( ( selectionBox.rect.w * selectionBox.rect.w )
                             + ( selectionBox.rect.h * selectionBox.rect.h ) );
    double x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
    ::ULIS::FVec2D pivot;

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT )
    {
        x1 = localCoords.x;
        y1 = localCoords.y;
        x2 = oldX2;
        y2 = oldY2;

        pivot.x = oldX2;
        pivot.y = oldY2;
    }

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT )
    {
        x1 = oldX1;
        y1 = localCoords.y;
        x2 = localCoords.x;
        y2 = oldY2;

        pivot.x = oldX1;
        pivot.y = oldY2;
    }

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT )
    {
        x1 = oldX1;
        y1 = oldY1;
        x2 = localCoords.x;
        y2 = localCoords.y;

        pivot.x = oldX1;
        pivot.y = oldY1;
    }

    if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT )
    {
        x1 = localCoords.x;
        y1 = oldY1;
        x2 = oldX2;
        y2 = localCoords.y;

        pivot.x = oldX2;
        pivot.y = oldY1;
    }

    //if( ( hudFlags & FOdysseyPainterEditorVectorObjectScaleToolHUD::HANDLE_MASK ) != 0 )
    {
        BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
        BLMatrix2D inverseSpaceMatrix;
        double x2mx1 = ( x2 - x1 );
        double y2my1 = ( y2 - y1 );
        BLMatrix2D scalingMatrix;

        spaceMatrix.translate( pivot.x, pivot.y );

        BLMatrix2D::invert( inverseSpaceMatrix, spaceMatrix );

        scalingMatrix.reset();

        if( Uniform )
        {
            double newDiagonal = sqrt( ( x2mx1 * x2mx1 ) + ( y2my1 * y2my1 ) );
            double ratio = newDiagonal / oldDiagonal;

            scalingMatrix.scale( ratio, ratio );
        }
        else
        {
            scalingMatrix.scale( x2mx1 / selectionBox.rect.w, y2my1 / selectionBox.rect.h );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            double selectionBoxArea = selectionBox.rect.Area();
             // side note: the sqrt() is there because surface rises at the square of dimension factor. We have to correct that.
            double radiusRatio = selectionBoxArea ? sqrt ( ( x2mx1 * y2my1 ) / selectionBoxArea ) : 1.0f;

            for( int i = 0; i < mTransformedVertexArray.size(); i++ )
            {
                TransformPoint( mTransformedVertexArray[i]
                              , spaceMatrix
                              , inverseSpaceMatrix
                              , scalingMatrix );

                mTransformedVertexArray[i]->SetRadius( mTransformedVertexArray[i]->GetRadius() * radiusRatio );
            }

            for( int i = 0; i < mTransformedHandleArray.size(); i++ )
            {
                TransformPoint( mTransformedHandleArray[i]
                              , spaceMatrix
                              , inverseSpaceMatrix
                              , scalingMatrix );
            }
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            // run lambda recursively on altered objects
            iEngine->Traverse
            ( iScene
            , iScene
            , 0
            ,[ iScene
             , iEngine
             , &spaceMatrix
             , &inverseSpaceMatrix
             , &scalingMatrix ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
             {
                  // transform is recursive per se, do not recurse if the parent was transformed already
                  if( ( travesalFlags & FOdysseyVectorEngine::TRAVERSE_PARENT_HASFOCUS ) == 0 )
                  {
                      if( iEngine->ObjectHasFocus( iScene, object, travesalFlags ) )
                      {
                          double translationX;
                          double translationY;
                          double rotation;
                          double scalingX;
                          double scalingY;
                          BLMatrix2D objectSpaceMatrix;
                          BLMatrix2D objectScaledMatrix;
                          BLMatrix2D objectLocalMatrix;
                          BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();
                          BLMatrix2D parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();

                          // transfer object in "Scaling Space" coordinates system
                          FOdysseyVector::MatrixMultiply( inverseSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                          // scale the object (local to the "Scaling Space" coordinates system)
                          FOdysseyVector::MatrixMultiply( scalingMatrix, objectSpaceMatrix, objectScaledMatrix );

                          // transfer the object back to world coordinates system
                          FOdysseyVector::MatrixMultiply( spaceMatrix, objectScaledMatrix, objectWorldMatrix );

                          // Convert the object to its parent coordinate system, i.e its local coordinates system.
                          FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

                          // Extract the local transformations
                          FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                                , &translationX
                                                                , &translationY
                                                                , &rotation // in radians
                                                                , &scalingX
                                                                , &scalingY );

                          // Apply the local transformations
                          object->Translate( translationX, translationY );
                          // for some reasons this affects the rotation, so we ignore it.
                          object->Rotate( /*rotation / M_PI * 180*/object->GetRotation() );
                          object->Scale( scalingX, scalingY );

                          object->UpdateMatrix();

                          return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
                      }
                  }

                  return 0;
              } );
        }
    }

    // Update the matrix for all objects
    //iScene->UpdateMatrix();

    // update the selection box with the newly modified matrices
    iEngine->ResetHUD();
}

uint64
UOdysseyPainterEditorVectorTransformTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        FOdysseyVectorEngine* iEngine = iScene->GetEngine();
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
        uint32 hudFlags = mTransformHUD->GetFlags();
        FVector2D currCursorPos = FSlateApplication::Get().GetCursorPos();
        FVector2D deltaPos = currCursorPos - mScreenMouseAtDown;

        // What do we consider dragging ? We have to move at least a few pixels, otherwise we wouldn't
        // be able to differentiate an actual dragging from a simple down-up click, especially
        // when using the stylus, which is too sensitive to allow a Down and a UP at the very same
        // position, unlike the mouse. And we use the simple down-up click as a selection tool.
        // See OnMouseUpVector() for details.
        if( ( mDragging == true ) // don't bother checking if we are already dragging
         || ( ::ULIS::FVec2D( deltaPos.X, deltaPos.Y ).Distance() > 3.0f ) )
        {
            mDragging = true;

            if( selectionBox.rect.Area() )
            {
                if ( mPickedPivot )
                {
                    BLMatrix2D& inverseWorldMatrix = selectionBox.inverseWorldMatrix;
                    BLPoint localCoords = inverseWorldMatrix.mapPoint( iPointInTexture.x, iPointInTexture.y );

                    mTransformHUD->SetGizmo( localCoords.x, localCoords.y );
                }
                else
                {
                    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS     )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS     )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
                    {
                        TranslateObjectSelection( iEngine, iScene, iPointInTexture );
                    }
                    else
                    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
                    {
                        RotateObjectSelection( iEngine, iScene, iPointInTexture );
                    }
                    else
                    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT     )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT    )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT  ) )
                    {
                        ScaleObjectSelection( iEngine, iScene, iPointInTexture );
                    }
                }
            }

            return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                 /*| FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED*/;
        }
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorTransformTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey)
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        mTransformHUD->ShowSelectionBox( true );

        if( mDragging == false )
        {
            // use the pick tool if the Down and Up events were at the same position (no dragging )
            GetEditor()->GetVectorSelectionTool()->OnMouseDown( iPointInTexture, iKey );
            GetEditor()->GetVectorSelectionTool()->OnMouseUp( iPointInTexture, iKey );

            mTransformHUD->CenterGizmo();

            // cancel the undo object that we created in the down event.
            if( mUndo )
            {
                delete mUndo;
            }
        }
        else
        {
            if( mUndo )
            {
                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("vector-transform-tool.transaction.transform","Vector Transform Tool"));
                if( GUndo )
                {
                    GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(mUndo) );
                }
                GEditor->EndTransaction();
            }

            iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

            // quick fix to place the gizmo at the right place
            FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
            ::ULIS::FVec2D& gizmo = mTransformHUD->GetGizmo(); 
            BLPoint worldGizmo = selectionBox.worldMatrix.mapPoint( gizmo.x, gizmo.y );
            // endof quickfix

            iEngine->ResetHUD();

            // quick fix to place the gizmo at the right place
            BLPoint localGizmo = selectionBox.inverseWorldMatrix.mapPoint( worldGizmo );
            mTransformHUD->SetGizmo( localGizmo.x, localGizmo.y );
            // endof quickfix
        }

        mUndo = nullptr;
        mDragging = false;
        mPickedPivot = nullptr;
    }

    mTransformHUD->SetCenterGizmo( true );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED;
}

uint64
UOdysseyPainterEditorVectorTransformTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                               , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();
    //mTransformHUD->MakeTransform( iScene, DivisionsX, DivisionsY );

    if( iPropertyName == "World" )
    {
        iEngine->ResetHUD();
        mTransformHUD->CenterGizmo();
    }

    return UOdysseyPainterEditorVectorSelectionTool::PropertyChangedVector( iScene, iPropertyName )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

TSharedRef<SWidget>
UOdysseyPainterEditorVectorTransformTool::CreateTopTabWidget()
{
    // return the BaseTool top tab instead the SelectionTool top tab (which is the base class for this class).
    return UOdysseyPainterEditorVectorBaseTool::CreateTopTabWidget();
}

#undef LOCTEXT_NAMESPACE
