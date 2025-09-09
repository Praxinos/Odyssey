// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditorSource.h"
#include "ISinglePropertyView.h"
#include "SOdysseySinglePropertyView.h"
// Vector engine
#include "OdysseyVector.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorObject.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerTransform.h"

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
    : UOdysseyPainterEditorVectorBaseTool( MakeShared<FOdysseyPainterEditorVectorTransformToolHUD>( this ), false, true )
    , mPickedPivot( nullptr )
    , mDragging( false )
    , ShowInbetweens( eShowInbetweens::Surrounding )
    , PickingRadius(10.0f)
    , Uniform( true )
    , ScalingCenter( ETransformToolScalingCenter::OppositeCorner )
    //, KeepPathWidth( false )
    , World( false )
    , bInbetweenMode( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform32");

    mTransformHUD = static_cast<FOdysseyPainterEditorVectorTransformToolHUD*>( mBaseHUD.Get() );
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
    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    // Note: we use mworkingLayer because is not called in the same context as Load, so there could be
    // an orphan cell here in the case of a cell deletion for example.
    mWorkingLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    if( iScene->GetCell()->GetLayer() == mWorkingLayer )
    {
        mWorkingLayer->RequestRedraw( iScene->GetCell(), 0 );
    }

    return 0;
}

uint64
UOdysseyPainterEditorVectorTransformTool::LoadVector( FOdysseyVectorGroupPaint* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
    uint32 hudFlags = mEditor->GetVectorHUDFlags();

    viewportWidget = viewportTab->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    mTransformHUD->CenterGizmo();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // force redrawing when we switch tool
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    bInbetweenMode = ( hudFlags & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? true : false;

    return 0;
}

bool
UOdysseyPainterEditorVectorTransformTool::OnKeyDownVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FKey& iKey )
{
    UniformAtKeyDown = Uniform;

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        Uniform = !Uniform; // flip the value
        return true;
    }

    return UOdysseyPainterEditorVectorBaseTool::OnKeyDownVector( iScene, iKey );
}

bool
UOdysseyPainterEditorVectorTransformTool::OnKeyUpVector( FOdysseyVectorGroupPaint* iScene
                                                       , const FKey& iKey )
{
    Uniform = UniformAtKeyDown;

    return UOdysseyPainterEditorVectorBaseTool::OnKeyUpVector( iScene, iKey );
}

void
UOdysseyPainterEditorVectorTransformTool::OnMouseHoverVector( FOdysseyVectorGroupPaint* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectD redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectD imageRegion;

    if( mDragging == false )
    {
        uint32 width = iScene->GetCell()->GetLayer()->GetWidth();
        uint32 height = iScene->GetCell()->GetLayer()->GetHeight();

        imageRegion.x = 0;
        imageRegion.y = 0;
        imageRegion.w = width;
        imageRegion.h = height;

        mTransformHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );
    }


    if( redrawRegion.Area() )
    {
        //iEngine->InvalidateRect( redrawRegion );

        //iEngine->GetInvalidTileMap().Invalidate(redrawRegion);
        // redraw
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::GetTransformedObjectList( FOdysseyVectorGroupPaint* iScene
                                                                  , std::list<FOdysseyVectorObject*>& oObjectList )
{
    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ iScene
      , &oObjectList ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          // transform is recursive per se, do not recurse if the parent was transformed already
          if( ( travesalFlags & FOdysseyVectorObject::TRAVERSE_PARENT_HASFOCUS ) == 0 )
          {
              if( iScene->GetCell()->ObjectHasFocus( object, travesalFlags ) )
              {
                  oObjectList.push_back( object );

                  return FOdysseyVectorObject::TRAVERSE_OBJECT_ACCEPTED;
              }
          }

          return 0;
      } );
}

void
UOdysseyPainterEditorVectorTransformTool::OnVectorLayerUpdate( const FOdysseyVectorObjectInvalidationFlags& iInvalidationFlags
                                                             , uint32 iUpdateFlags )
{
    if( ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TOPOLOGY] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_SHAPE] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::TAG_INBETWEENER_MATRIX] )
     || ( iInvalidationFlags.bits[FOdysseyVectorObjectInvalidationFlags::CHILD_TAG_INBETWEENER_MATRIX] ) )
    {
        if( mBaseHUD )
        {
            mBaseHUD->Reset();
        }
    }

    // will react to OBJECT_SELECTION and CHILD_OBJECT_SELECTION.
    // Will also Reset the HUD. the HUD in that case might be reset twice
    UOdysseyPainterEditorVectorBaseTool::OnVectorLayerUpdate( iInvalidationFlags, iUpdateFlags );
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseDownVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    mTransformHUD->SetCenterGizmo( false );

    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
        uint32 hudFlags = mTransformHUD->GetFlags();

        mDragging = false;
        mScreenMouseAtDown = FSlateApplication::Get().GetCursorPos();
        mMouseAtDown.x = iPointInTexture.x;
        mMouseAtDown.y = iPointInTexture.y;

        mPickedPivot = hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ZAXIS ? &mTransformHUD->GetGizmo() : nullptr;

        MakeSpaceMatrix();

        mTransformedObjectBuffer.clear();
        mTransformedBreakdownBuffer.clear();

        if( ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        ||  ( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT ) )
        {
            FOdysseyVectorObject::Traverse
            ( iScene
            , 0
            , [ iScene
              , this ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
              {
                  if( iScene->GetCell()->ObjectHasFocus( object, travesalFlags ) )
                  {
                      BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();
                      BLMatrix2D parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();
                      TransformedObject& transformedObject = mTransformedObjectBuffer.emplace_back();

                      transformedObject.object = object;
                      transformedObject.worldMatrix = object->GetWorldMatrix();
                      transformedObject.parentInverseWorldMatrix = object->GetParent()->GetInverseWorldMatrix();

                      // transform is recursive per se, do not recurse if the parent was transformed already
                      return FOdysseyVectorObject::TRAVERSE_OBJECT_IGNORE_CHILDREN;
                  }

                  return 0;
              } );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            for( FInbetweenerBreakdown* breakdown : mTransformHUD->GetSelectedBreakdownList() )
            {
                TransformedBreakdown& transformedBreakdown = mTransformedBreakdownBuffer.emplace_back();

                transformedBreakdown.breakdown = breakdown;
                transformedBreakdown.worldMatrix = breakdown->GetTargetWorldMatrix();
                transformedBreakdown.ownerInverseWorldMatrix = breakdown->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
            }
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            mTransformedVertexArray.clear();
            mTransformedVertexPositionArray.clear();
            mTransformedHandleArray.clear();
            mTransformedHandlePositionArray.clear();

            GetSelectedVertices( iScene, mTransformedVertexArray );
            // static call
            UOdysseyPainterEditorVectorBaseTool::GetSegmentHandlesFromVertices( mTransformedVertexArray
                                                                              , mTransformedHandleArray );

            // Get vertices'original position
            mTransformedVertexPositionArray.reserve( mTransformedVertexArray.size() );
            for( FOdysseyVectorVertex* vertex : mTransformedVertexArray )
            {
                mTransformedVertexPositionArray.push_back( vertex->GetCoords() );
            }

            // Get handles'original position
            mTransformedHandlePositionArray.reserve( mTransformedVertexArray.size() );
            for( FOdysseyVectorHandleSegment* handle : mTransformedHandleArray )
            {
                mTransformedHandlePositionArray.push_back( handle->GetCoords() );
            }

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
            mUndo = new FOdysseyVectorUndoObjectTransform( iScene
                                                         , transformedObjectList );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            // remember for undos. we don't register the undo in the mouse down event yet because
            // it could conflict with the undo created by th emouse up event in the case of a no-drag
            mUndo = new FOdysseyVectorUndoTagInbetweenerTransform( iScene
                                                                 , mTransformHUD->GetSelectedBreakdownList() );
        }

        if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
        {
            mTransformHUD->ShowSelectionBox( false );
        }
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorTransformTool::TransformGizmo( BLMatrix2D& iTransformationMatrix )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    ::ULIS::FVec2D& gizmo = mTransformHUD->GetGizmo();

    if( selectionBox.rect.Area() )
    {
        ::ULIS::FVec2D gizmoSpaceTransformedCoords = FOdysseyVector::MapPoint( iTransformationMatrix, mSpaceGizmo );
        ::ULIS::FVec2D newWorldCoords  = FOdysseyVector::MapPoint( mSpaceMatrix, gizmoSpaceTransformedCoords );

        gizmo = FOdysseyVector::MapPoint( selectionBox.inverseWorldMatrix, newWorldCoords );
    }
}

static void
TransformPoint( FOdysseyVectorPoint* iPoint
              , ::ULIS::FVec2D& iOriginalPosition
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
        BLPoint worldCoords = ownerObject->GetWorldMatrix().mapPoint( iOriginalPosition.x, iOriginalPosition.y );
        BLPoint spaceCoords = iInverseSpaceMatrix.mapPoint( worldCoords.x, worldCoords.y );
        BLPoint transCoords = iTransformationMatrix.mapPoint( spaceCoords.x, spaceCoords.y );
        BLPoint newWorldCoords = iSpaceMatrix.mapPoint( transCoords.x, transCoords.y );
        BLPoint newLocalCoords = ownerObject->GetInverseWorldMatrix().mapPoint( newWorldCoords.x, newWorldCoords.y );

        iPoint->Set( newLocalCoords.x, newLocalCoords.y );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::TranslateObjectSelection( FOdysseyVectorGroupPaint* iScene
                                                                  , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FVec2D oldLocalCoords = FOdysseyVector::MapPoint( mInverseSpaceMatrix, ::ULIS::FVec2D( mMouseAtDown.x
                                                                                                 , mMouseAtDown.y ) );
    ::ULIS::FVec2D localCoords = FOdysseyVector::MapPoint( mInverseSpaceMatrix, ::ULIS::FVec2D( iPointInTexture.x
                                                                                              , iPointInTexture.y ) );
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    uint32 hudFlags = mTransformHUD->GetFlags();
    BLMatrix2D translateMatrix;
    BLPoint translateBy(0,0);
    ::ULIS::FVec2D localDelta = localCoords - oldLocalCoords;

    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS )
    {
        translateBy.x = localDelta.x;
    }

    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS )
    {
        translateBy.y = localDelta.y;
    }

    translateMatrix.resetToTranslation( translateBy.x, translateBy.y );

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        for( int i = 0; i < mTransformedVertexArray.size(); i++ )
        {
            TransformPoint( mTransformedVertexArray[i]
                          , mTransformedVertexPositionArray[i]
                          , mSpaceMatrix
                          , mInverseSpaceMatrix
                          , translateMatrix );
        }

        for( int i = 0; i < mTransformedHandleArray.size(); i++ )
        {
            TransformPoint( mTransformedHandleArray[i]
                          , mTransformedHandlePositionArray[i]
                          , mSpaceMatrix
                          , mInverseSpaceMatrix
                          , translateMatrix );
        }

        // this will also reset the HUD due to the object's shape being modified
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );

    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        for( TransformedObject& transformedObject : mTransformedObjectBuffer )
        {
            FOdysseyVectorObject* object = transformedObject.object;
            double translationX;
            double translationY;
            double rotation;
            double scalingX;
            double scalingY;
            double skewX;
            double skewY;
            BLMatrix2D objectSpaceMatrix;
            BLMatrix2D objectSpaceTransformedMatrix;
            BLMatrix2D objectLocalMatrix;
            BLMatrix2D objectWorldMatrix = transformedObject.worldMatrix;
            BLMatrix2D parentInverseWorldMatrix = transformedObject.parentInverseWorldMatrix;

            // transfer object in "Selection Space" coordinates system
            FOdysseyVector::MatrixMultiply( mInverseSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

            // translate the object (local to the "Selection Space" coordinates system)
            FOdysseyVector::MatrixMultiply( translateMatrix, objectSpaceMatrix, objectSpaceTransformedMatrix );

            // transfer the object back to world coordinates system
            FOdysseyVector::MatrixMultiply( mSpaceMatrix, objectSpaceTransformedMatrix, objectWorldMatrix );

            // Convert the object to its parent coordinate system, i.e its local coordinates system.
            FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

            // Extract the local transformations
            FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                , &translationX
                                                , &translationY
                                                , &rotation // in radians
                                                , &scalingX
                                                , &scalingY
                                                , &skewX
                                                , &skewY
                                                , false );

            // Apply the local transformations
            object->Translate( translationX, translationY );
            object->Rotate( rotation / M_PI * 180 ); // in degrees
            object->Scale( scalingX, scalingY );
            object->Skew( skewX, skewY );

            object->UpdateMatrix();
        }

        // this will also reset the HUD due to the object's matrix being modified
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        for( TransformedBreakdown& transformedBreakdown : mTransformedBreakdownBuffer )
        {
            double translationX;
            double translationY;
            double rotation;
            double scalingX;
            double scalingY;
            double skewX;
            double skewY;
            BLMatrix2D breakdownSpaceMatrix;
            BLMatrix2D breakdownTranslateMatrix;
            BLMatrix2D breakdownLocalMatrix;
            FInbetweenerBreakdown* breakdown = transformedBreakdown.breakdown;
            BLMatrix2D breakdownWorldMatrix = transformedBreakdown.worldMatrix;
            BLMatrix2D ownerInverseWorldMatrix = transformedBreakdown.ownerInverseWorldMatrix;

            // transfer object in "Selection Space" coordinates system
            FOdysseyVector::MatrixMultiply( mInverseSpaceMatrix, breakdownWorldMatrix, breakdownSpaceMatrix );

            // translate the object (local to the "Selection Space" coordinates system)
            FOdysseyVector::MatrixMultiply( translateMatrix, breakdownSpaceMatrix, breakdownTranslateMatrix );

            // transfer the object back to world coordinates system
            FOdysseyVector::MatrixMultiply( mSpaceMatrix, breakdownTranslateMatrix, breakdownWorldMatrix );

            // Convert the object to its parent coordinate system, i.e its local coordinates system.
            FOdysseyVector::MatrixMultiply( ownerInverseWorldMatrix, breakdownWorldMatrix, breakdownLocalMatrix );

            // Extract the local transformations
            FOdysseyVector::ExtractTransformations( breakdownLocalMatrix
                                                  , &translationX
                                                  , &translationY
                                                  , &rotation // in radians
                                                  , &scalingX
                                                  , &scalingY
                                                  , &skewX
                                                  , &skewY
                                                  , false );

            // Apply the local transformations
            breakdown->Translate( translationX, translationY );
            breakdown->Rotate( rotation / M_PI * 180 ); // in degrees
            breakdown->Scale( scalingX, scalingY );
            breakdown->Skew( skewX, skewY );

            breakdown->UpdateMatrix();
        }

        // this will also reset the HUD due to the tag's matrix being modified
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    }

    TransformGizmo( translateMatrix );
}

double
UOdysseyPainterEditorVectorTransformTool::GetRotationAngle( const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    ::ULIS::FVec2D pt[2] = { FOdysseyVector::MapPoint( mInverseSpaceMatrix, mMouseAtDown )
                           , FOdysseyVector::MapPoint( mInverseSpaceMatrix, ::ULIS::FVec2D( iPointInTexture.x
                                                                                          , iPointInTexture.y ) ) };
    ::ULIS::FVec2D worldGizmo = FOdysseyVector::MapPoint( selectionBox.worldMatrix, mTransformHUD->GetGizmo() );
    ::ULIS::FVec2D spaceGizmo = FOdysseyVector::MapPoint( mInverseSpaceMatrix, worldGizmo );
    ::ULIS::FVec2D vector[2];
    double angle = 0.0f;

    vector[0].x = pt[0].x - spaceGizmo.x;
    vector[0].y = pt[0].y - spaceGizmo.y;

    vector[1].x = pt[1].x - spaceGizmo.x;
    vector[1].y = pt[1].y - spaceGizmo.y;

    if( vector[0].DistanceSquared() )
    {
        vector[0].Normalize();
    }

    if( vector[1].DistanceSquared() )
    {
        vector[1].Normalize();
    }

    angle = ( FOdysseyVector::Cross2D( vector[0], vector[1] ) > 0.0f ) ?  acos( vector[0].DotProduct( vector[1] ) )
                                                                       : -acos( vector[0].DotProduct( vector[1] ) ) + ( 2 * M_PI );

    return angle;
}

void
UOdysseyPainterEditorVectorTransformTool::RotateObjectSelection( FOdysseyVectorGroupPaint* iScene
                                                               , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    BLMatrix2D rotateMatrix;
    double rotationAngle = GetRotationAngle( iPointInTexture );

    rotateMatrix.resetToRotation( rotationAngle ); // Radians

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
    {
        for( int i = 0; i < mTransformedVertexArray.size(); i++ )
        {
            TransformPoint( mTransformedVertexArray[i]
                          , mTransformedVertexPositionArray[i]
                          , mSpaceMatrix
                          , mInverseSpaceMatrix
                          , rotateMatrix );
        }

        for( int i = 0; i < mTransformedHandleArray.size(); i++ )
        {
            TransformPoint( mTransformedHandleArray[i]
                          , mTransformedHandlePositionArray[i]
                          , mSpaceMatrix
                          , mInverseSpaceMatrix
                          , rotateMatrix );
        }

        // this will also reset the HUD due to the object's shape being modified
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
    {
        for( TransformedObject& transformedObject : mTransformedObjectBuffer )
        {
            FOdysseyVectorObject* object = transformedObject.object;
            double translationX;
            double translationY;
            double rotation;
            double scalingX;
            double scalingY;
            double skewX;
            double skewY;
            BLMatrix2D objectSpaceMatrix;
            BLMatrix2D objectSpaceTransformedMatrix;
            BLMatrix2D objectLocalMatrix;
            BLMatrix2D objectWorldMatrix = transformedObject.worldMatrix;
            BLMatrix2D parentInverseWorldMatrix = transformedObject.parentInverseWorldMatrix;

            // transfer object in "Rotation Space" coordinates system
            FOdysseyVector::MatrixMultiply( mInverseSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

            // rotate the object (local to the "Rotation Space" coordinates system)
            FOdysseyVector::MatrixMultiply( rotateMatrix, objectSpaceMatrix, objectSpaceTransformedMatrix );

            // transfer the object back to world coordinates system
            FOdysseyVector::MatrixMultiply( mSpaceMatrix, objectSpaceTransformedMatrix, objectWorldMatrix );

            // Convert the object to its parent coordinate system, i.e its local coordinates system.
            FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

            // Extract the local transformations
            FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                  , &translationX
                                                  , &translationY
                                                  , &rotation // in radians
                                                  , &scalingX
                                                  , &scalingY
                                                  , &skewX
                                                  , &skewY
                                                  , false );

            // Apply the local transformations
            object->Translate( translationX, translationY );
            object->Rotate( rotation / M_PI * 180.0f ); // in degrees
            object->Scale( scalingX, scalingY );
            object->Skew( skewX, skewY );

            object->UpdateMatrix();
        }

        // this will also reset the HUD due to the object's matrix being modified
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                  | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
    }

    if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
    {
        for( TransformedBreakdown& transformedBreakdown : mTransformedBreakdownBuffer )
        {
            double translationX;
            double translationY;
            double rotation;
            double scalingX;
            double scalingY;
            double skewX;
            double skewY;
            BLMatrix2D breakdownSpaceMatrix;
            BLMatrix2D breakdownSpaceTransformedMatrix;
            BLMatrix2D breakdownLocalMatrix;
            FInbetweenerBreakdown* breakdown = transformedBreakdown.breakdown;
            BLMatrix2D breakdownWorldMatrix = transformedBreakdown.worldMatrix;
            BLMatrix2D ownerInverseWorldMatrix = transformedBreakdown.ownerInverseWorldMatrix;

            // transfer object in "Rotation Space" coordinates system
            FOdysseyVector::MatrixMultiply( mInverseSpaceMatrix, breakdownWorldMatrix, breakdownSpaceMatrix );

            // rotate the object (local to the "Rotation Space" coordinates system)
            FOdysseyVector::MatrixMultiply( rotateMatrix, breakdownSpaceMatrix, breakdownSpaceTransformedMatrix );

            // transfer the object back to world coordinates system
            FOdysseyVector::MatrixMultiply( mSpaceMatrix, breakdownSpaceTransformedMatrix, breakdownWorldMatrix );

            // Convert the object to its parent coordinate system, i.e its local coordinates system.
            FOdysseyVector::MatrixMultiply( ownerInverseWorldMatrix, breakdownWorldMatrix, breakdownLocalMatrix );

            // Extract the local transformations
            FOdysseyVector::ExtractTransformations( breakdownLocalMatrix
                                                  , &translationX
                                                  , &translationY
                                                  , &rotation // in radians
                                                  , &scalingX
                                                  , &scalingY
                                                  , &skewX
                                                  , &skewY
                                                  , false );

            // Apply the local transformations
            breakdown->Translate( translationX, translationY );
            breakdown->Rotate( rotation / M_PI * 180.0f ); // in degrees
            breakdown->Scale( scalingX, scalingY );
            breakdown->Skew( skewX, skewY );

            breakdown->UpdateMatrix();
        }

        // this will also reset the HUD due to the tag's matrix being modified
        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
    }

    TransformGizmo( rotateMatrix );
}

void
UOdysseyPainterEditorVectorTransformTool::MakeSpaceMatrix()
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    ::ULIS::FVec2D worldGizmo = FOdysseyVector::MapPoint( selectionBox.worldMatrix, mTransformHUD->GetGizmo() );
    uint32 hudFlags = mTransformHUD->GetFlags();

    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE )
    {
        MakeSpaceMatrixForTranslation();
    }

    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
    {
        MakeSpaceMatrixForRotation();
    }

    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALE )
    {
        MakeSpaceMatrixForScaling();
    }

    mSpaceGizmo = FOdysseyVector::MapPoint( mInverseSpaceMatrix, worldGizmo );
}

void
UOdysseyPainterEditorVectorTransformTool::MakeSpaceMatrixForRotation()
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    double selectionBoxSurface = selectionBox.rect.w * selectionBox.rect.h;
    BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
    ::ULIS::FVec2D& pivot = mTransformHUD->GetGizmo();
    BLPoint worldPivot = spaceMatrix.mapPoint( pivot.x, pivot.y );

    if( selectionBoxSurface )
    {
        spaceMatrix = selectionBox.worldMatrix;

        spaceMatrix.translate( pivot.x, pivot.y );
    }

    mSpaceMatrix = spaceMatrix;

    BLMatrix2D::invert( mInverseSpaceMatrix, spaceMatrix );
}

void
UOdysseyPainterEditorVectorTransformTool::MakeSpaceMatrixForTranslation()
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    double selectionBoxSurface = selectionBox.rect.w * selectionBox.rect.h;
    BLMatrix2D spaceMatrix = selectionBox.worldMatrix;

    if( selectionBoxSurface )
    {
        spaceMatrix = selectionBox.worldMatrix;
    }

    mSpaceMatrix = spaceMatrix;

    BLMatrix2D::invert( mInverseSpaceMatrix, spaceMatrix );
}

void
UOdysseyPainterEditorVectorTransformTool::MakeSpaceMatrixForScaling()
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    double selectionBoxSurface = selectionBox.rect.w * selectionBox.rect.h;
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    uint32 hudFlags = mTransformHUD->GetFlags();
    double oldX1 = selectionBox.rect.x
         , oldY1 = selectionBox.rect.y
         , oldX2 = selectionBox.rect.x + selectionBox.rect.w
         , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
    ::ULIS::FVec2D pivot = ::ULIS::FVec2D( 0.0f, 0.0f );
    BLMatrix2D spaceMatrix;

    switch( ScalingCenter )
    {
        case ETransformToolScalingCenter::OppositeCorner :
        {
            ::ULIS::FVec2D oppositeCorner;

            if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT )
            {
                oppositeCorner.x = oldX2;
                oppositeCorner.y = oldY2;
            }

            if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT )
            {
                oppositeCorner.x = oldX1;
                oppositeCorner.y = oldY2;
            }

            if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT )
            {
                oppositeCorner.x = oldX1;
                oppositeCorner.y = oldY1;
            }

            if ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT )
            {
                oppositeCorner.x = oldX2;
                oppositeCorner.y = oldY1;
            }

            pivot = oppositeCorner;
        }
        break;

        case ETransformToolScalingCenter::BoxCenter :
            pivot.x = oldX1 + ( oldX2 - oldX1 ) * 0.5f;
            pivot.y = oldY1 + ( oldY2 - oldY1 ) * 0.5f;
        break;

        case ETransformToolScalingCenter::Gizmo :
            pivot.x = mTransformHUD->GetGizmo().x;
            pivot.y = mTransformHUD->GetGizmo().y;
        break;

        default :
        break;
    }

    spaceMatrix.reset();

    if( selectionBoxSurface )
    {
        spaceMatrix = selectionBox.worldMatrix;

        // build a "space matrix", i.e the matrix that will serve as a "container" of all matrices that are inside.
        // objects will then have a relative matrix computed from this matrix and we will rescale that relative matrix.
        spaceMatrix.translate( pivot.x, pivot.y );
    }

    mSpaceMatrix = spaceMatrix;

    BLMatrix2D::invert( mInverseSpaceMatrix, spaceMatrix );
}

void
UOdysseyPainterEditorVectorTransformTool::ScaleObjectSelection( FOdysseyVectorGroupPaint* iScene
                                                              , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    uint32 hudFlags = mTransformHUD->GetFlags();
    ::ULIS::FVec2D oldLocalCoords = FOdysseyVector::MapPoint( mInverseSpaceMatrix, ::ULIS::FVec2D( mMouseAtDown.x
                                                                                                 , mMouseAtDown.y ) );
    ::ULIS::FVec2D localCoords = FOdysseyVector::MapPoint( mInverseSpaceMatrix, ::ULIS::FVec2D( iPointInTexture.x
                                                                                              , iPointInTexture.y ) );
    double difx = localCoords.x - oldLocalCoords.x;
    double dify = localCoords.y - oldLocalCoords.y;
    double oldX1 = selectionBox.rect.x
         , oldY1 = selectionBox.rect.y
         , oldX2 = selectionBox.rect.x + selectionBox.rect.w
         , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
    double selectionBoxSurface = selectionBox.rect.w * selectionBox.rect.h;

    if( selectionBoxSurface )
    {
        BLMatrix2D scalingMatrix;

        scalingMatrix.reset();

        if( Uniform )
        {
            //double ratio = (fabs(difx) > fabs(dify) ) ? 1.0f + ( difx / selectionBox.rect.w )
            //                                          : 1.0f + ( dify / selectionBox.rect.h );
            double ratio = ( localCoords ).Distance() / ( oldLocalCoords ).Distance();

            scalingMatrix.scale( ratio, ratio );
        }
        else
        {
            double xratio = ( localCoords.x ) / ( oldLocalCoords.x );
            double yratio = ( localCoords.y ) / ( oldLocalCoords.y );

            //if( ( xratio > 0.0f ) && ( yratio > 0.0f ) )
                scalingMatrix.scale( xratio, yratio );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_VERTEX )
        {
            double selectionBoxArea = selectionBox.rect.Area();
             // side note: the sqrt() is there because surface rises at the square of dimension factor. We have to correct that.
            double radiusRatio = /*selectionBoxArea ? sqrt ( ( x2mx1 * y2my1 ) / selectionBoxArea ) : 1.0f*/1.0f;

            for( int i = 0; i < mTransformedVertexArray.size(); i++ )
            {
                TransformPoint( mTransformedVertexArray[i]
                              , mTransformedVertexPositionArray[i]
                              , mSpaceMatrix
                              , mInverseSpaceMatrix
                              , scalingMatrix );

                mTransformedVertexArray[i]->SetRadius( mTransformedVertexArray[i]->GetRadius() * radiusRatio );
            }

            for( int i = 0; i < mTransformedHandleArray.size(); i++ )
            {
                TransformPoint( mTransformedHandleArray[i]
                              , mTransformedHandlePositionArray[i]
                              , mSpaceMatrix
                              , mInverseSpaceMatrix
                              , scalingMatrix );
            }

            // this will also reset the HUD due to the object's shape being modified
            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                      | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_OBJECT )
        {
            for( TransformedObject& transformedObject : mTransformedObjectBuffer )
            {
                FOdysseyVectorObject* object = transformedObject.object;
                double translationX;
                double translationY;
                double rotation;
                double scalingX;
                double scalingY;
                double skewX;
                double skewY;
                BLMatrix2D objectSpaceMatrix;
                BLMatrix2D objectScaledMatrix;
                BLMatrix2D objectLocalMatrix;
                BLMatrix2D objectWorldMatrix = transformedObject.worldMatrix;
                BLMatrix2D parentInverseWorldMatrix = transformedObject.parentInverseWorldMatrix;

                // transfer object in "Scaling Space" coordinates system
                FOdysseyVector::MatrixMultiply( mInverseSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                // scale the object (local to the "Scaling Space" coordinates system)
                FOdysseyVector::MatrixMultiply( scalingMatrix, objectSpaceMatrix, objectScaledMatrix );

                // transfer the object back to world coordinates system
                FOdysseyVector::MatrixMultiply( mSpaceMatrix, objectScaledMatrix, objectWorldMatrix );

                // Convert the object to its parent coordinate system, i.e its local coordinates system.
                FOdysseyVector::MatrixMultiply( parentInverseWorldMatrix, objectWorldMatrix, objectLocalMatrix );

                // Extract the local transformations
                FOdysseyVector::ExtractTransformations( objectLocalMatrix
                                                      , &translationX
                                                      , &translationY
                                                      , &rotation // in radians
                                                      , &scalingX
                                                      , &scalingY
                                                      , &skewX
                                                      , &skewY
                                                      , false );

                // Apply the local transformations
                object->Translate( translationX, translationY );
                // for some reasons this affects the rotation, so we ignore it.
                object->Rotate( rotation / M_PI * 180.0f );
                object->Scale( scalingX, scalingY );
                object->Skew( skewX, skewY );

                object->UpdateMatrix();
            }

            // this will also reset the HUD due to the object's matrix being modified
            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE
                                      | FOdysseyVectorObject::UPDATE_NOINBETWEENING );
        }

        if( mEditor->GetVectorHUDFlags() & FOdysseyVectorHUD::HUD_MODE_INBETWEEN )
        {
            for( TransformedBreakdown& transformedBreakdown : mTransformedBreakdownBuffer )
            {
                double translationX;
                double translationY;
                double rotation;
                double scalingX;
                double scalingY;
                double skewX;
                double skewY;
                BLMatrix2D breakdownSpaceMatrix;
                BLMatrix2D breakdownScalingMatrix;
                BLMatrix2D breakdownLocalMatrix;
                FInbetweenerBreakdown* breakdown = transformedBreakdown.breakdown;
                BLMatrix2D breakdownWorldMatrix = transformedBreakdown.worldMatrix;
                BLMatrix2D ownerInverseWorldMatrix = transformedBreakdown.ownerInverseWorldMatrix;

                // transfer object in "Rotation Space" coordinates system
                FOdysseyVector::MatrixMultiply( mInverseSpaceMatrix, breakdownWorldMatrix, breakdownSpaceMatrix );

                // rotate the object (local to the "Rotation Space" coordinates system)
                FOdysseyVector::MatrixMultiply( scalingMatrix, breakdownSpaceMatrix, breakdownScalingMatrix );

                // transfer the object back to world coordinates system
                FOdysseyVector::MatrixMultiply( mSpaceMatrix, breakdownScalingMatrix, breakdownWorldMatrix );

                // Convert the object to its parent coordinate system, i.e its local coordinates system.
                FOdysseyVector::MatrixMultiply( ownerInverseWorldMatrix, breakdownWorldMatrix, breakdownLocalMatrix );

                // Extract the local transformations
                FOdysseyVector::ExtractTransformations( breakdownLocalMatrix
                                                      , &translationX
                                                      , &translationY
                                                      , &rotation // in radians
                                                      , &scalingX
                                                      , &scalingY
                                                      , &skewX
                                                      , &skewY
                                                      , false );

                // Apply the local transformations
                breakdown->Translate( translationX, translationY );
                // for some reasons this affects the rotation, so we ignore it.
                breakdown->Rotate( breakdown->GetTargetRotation() ); // in degrees
                breakdown->Scale( scalingX, scalingY );
                breakdown->Skew( skewX, skewY );

                breakdown->UpdateMatrix();
            }

            // this will also reset the HUD due to the tag's matrix being modified
            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_INTERACTIVE );
        }

        TransformGizmo( scalingMatrix );
    }
}

void
UOdysseyPainterEditorVectorTransformTool::OnMouseDragVector( FOdysseyVectorGroupPaint* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    static FVector2D deltaPositionCumul = FVector2D( 0.0f, 0.0f );
    FOdysseyPoint pointInTexture = iPointInTexture;

    // because we ignore some events, we need to accumulate the delta
    deltaPositionCumul += iPointInTexture.deltaPosition;

    // For some reason we receive quite a lot of mouse events between 2 screen refresh, I don't know why
    // The issue is absent with the Ink driver. It is present with the Wintab and Native drivers. The simpliest
    // solution I've found is to discard events until the screen has been refreshed.
    if( iScene->GetCell()->PendingRedraw()  )
        return;

    pointInTexture.deltaPosition = deltaPositionCumul;

    // Left mouse button clicked
    if( pointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        FVector2D currCursorPos = FSlateApplication::Get().GetCursorPos();
        FVector2D deltaPos = currCursorPos - mScreenMouseAtDown;
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
        uint32 hudFlags = mTransformHUD->GetFlags();

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
                    BLPoint localCoords = inverseWorldMatrix.mapPoint( pointInTexture.x, pointInTexture.y );

                    mTransformHUD->SetGizmo( localCoords.x, localCoords.y );
                }
                else
                {
                    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS ) )
                    {
                        TranslateObjectSelection( iScene, pointInTexture );
                    }
                    else
                    if( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ROTATE )
                    {
                        RotateObjectSelection( iScene, pointInTexture );
                    }
                    else
                    if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPLEFT     )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_TOPRIGHT    )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMRIGHT )
                     || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_SCALER_BOTTOMLEFT  ) )
                    {
                        ScaleObjectSelection( iScene, pointInTexture );
                    }
                }
            }
        }

        // redraw whole screen
        iScene->GetCell()->InvalidateRect();

        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), FOdysseyVectorCell::REDRAW_INTERACTIVE );
    }

    deltaPositionCumul = FVector2D( 0.0f, 0.0f );
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseUpVector( FOdysseyVectorGroupPaint* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

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

                    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
                    if (source)
                        source->RecordCurrentFrameUndo();
                }
                GEditor->EndTransaction();
            }

            // update invalidated objects. Updating via shared Env will invalidate the engine, thus redrawing the image
            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

            // quick fix to place the gizmo at the right place
            FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
            ::ULIS::FVec2D& gizmo = mTransformHUD->GetGizmo();
            BLPoint worldGizmo = selectionBox.worldMatrix.mapPoint( gizmo.x, gizmo.y );
            // endof quickfix

            iScene->GetLayer()->ResetHUD( iScene );

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

    return true;
}

void
UOdysseyPainterEditorVectorTransformTool::PropertyChangedVector( FOdysseyVectorGroupPaint* iScene
                                                               , const FName& iPropertyName )
{
    //mTransformHUD->MakeTransform( iScene, DivisionsX, DivisionsY );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorTransformTool, World) )
    {
        iScene->GetLayer()->ResetHUD( iScene );

        mTransformHUD->CenterGizmo();
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName );
}

EVisibility
UOdysseyPainterEditorVectorTransformTool::IsModeInbetween() const
{
    uint32 hudFlags = mEditor->GetVectorHUDFlags();

    return ( hudFlags  & FOdysseyVectorHUD::HUD_MODE_INBETWEEN ) ? EVisibility::Visible
                                                                 : EVisibility::Collapsed;
}

void
UOdysseyPainterEditorVectorTransformTool::ExtendToolbar( FToolBarBuilder& iBuilder )
{
    Super::ExtendToolbar(iBuilder);

    iBuilder.BeginSection( NAME_None );

    TAttribute<EVisibility> value = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateUObject (this, &UOdysseyPainterEditorVectorTransformTool::IsModeInbetween) );

    iBuilder.AddWidget(
        SNew(SBox)
        .Visibility_UObject(this, &UOdysseyPainterEditorVectorTransformTool::IsModeInbetween)
        .Padding(10.f, 0.f, 10.f, 0.f)
        [
            SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTransformTool, ShowInbetweens ), FSinglePropertyParams())
            .InnerPadding(10.f)
        ]
    );

    iBuilder.EndSection();
}

FText
UOdysseyPainterEditorVectorTransformTool::GetTooltip() const
{
    return LOCTEXT("vector-transform-tool.tooltip", "Transform Tool");
}

#undef LOCTEXT_NAMESPACE
