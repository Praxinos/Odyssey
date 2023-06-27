// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformToolHUD.h"
#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorTransformTool"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorTransformTool::~UOdysseyPainterEditorVectorTransformTool()
{
}

UOdysseyPainterEditorVectorTransformTool::UOdysseyPainterEditorVectorTransformTool()
    : PickingRadius( 10.0f )
    , mPickedPivot( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Transform64");

    mTransformHUD = new FOdysseyPainterEditorVectorTransformToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorTransformTool::UnloadVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mTransformHUD );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorTransformTool::LoadVector( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mTransformHUD );

    mTransformHUD->Reset( iScene );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

::ULIS::FRectI
UOdysseyPainterEditorVectorTransformTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                            , FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    ::ULIS::FRectI imageRegion;

    iEngine->GetColorImageSize( imageRegion );

    if( mTransformHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y ) )
    {
        redrawRegion = imageRegion; // redraw the whole image when the gizmo must be redrawn
    }

    return redrawRegion;
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture
                                                           , const FKey& iKey )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

    mPickedPivot = mTransformHUD->GetFlags() & FOdysseyPainterEditorVectorTransformToolHUD::PICK_ZAXIS ? &mTransformHUD->GetGizmo() : nullptr;

    if( selectionBox.rect.Area() )
    {
        if( mPickedPivot )
        {

        }
/*
        if( mPickedPivot == nullptr )
        {
            BLPoint pt = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            ::ULIS::FVec2D& pivot = mObjectRotateHUD->GetPivot();

            mStartAtVector.x = pt.x - pivot.x;
            mStartAtVector.y = pt.y - pivot.y;

            if( mStartAtVector.DistanceSquared() )
            {
                mStartAtVector.Normalize();
            }
        }
*/
    }

    return true;
}

void
UOdysseyPainterEditorVectorTransformTool::MoveObjectSelection( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    //::ULIS::FRectD beforeBBox = FOdysseyVectorObject::GetBoundingBoxFromList( selectedObjectList );
    //::ULIS::FRectI redrawRegion = { 0, 0, 0, 0 };
    uint32 hudFlags = mTransformHUD->GetFlags();
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();
    ::ULIS::FVec2D& gizmo = mTransformHUD->GetGizmo();

    if( selectionBox.rect.Area() )
    {
        BLMatrix2D spaceMatrix = selectionBox.worldMatrix;
        BLMatrix2D invertSpaceMatrix;
        BLMatrix2D translateMatrix;
        BLPoint translateBy;

        BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

        translateBy = invertSpaceMatrix.mapVector( iPointInTexture.deltaPosition.X
                                                 , iPointInTexture.deltaPosition.Y );

        translateMatrix.reset();

        if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_XAXIS     )
         || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
        {
            translateMatrix.translate( translateBy.x, 0 );
        }

        if( ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_YAXIS     )
         || ( hudFlags & FOdysseyPainterEditorVectorTransformToolHUD::PICK_TRANSLATE ) )
        {
            translateMatrix.translate( 0, translateBy.y );
        }

        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* object = (*it);

            if ( object->HasSelectedAncestor() == false )
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
                FOdysseyVector::MatrixMultiply( invertSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

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
            }
        }

        // Update the matrix for all objects
        iScene->UpdateMatrix();

        // update the selection box with the newly modified matrices
        mTransformHUD->UpdateSelectionBox( iScene, World );

        //mTransformHUD->SetGizmo( gizmo.x + translateBy.x, gizmo.y + translateBy.y ); 
        //mObjectMoveHUD->Reset( iScene ); // Updates selection box and gizmo position
    }
}

void
UOdysseyPainterEditorVectorTransformTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

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
            MoveObjectSelection( iEngine, iScene, iPointInTexture );
        }
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorTransformTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture
                                                         , const FKey& iKey )
{

    mPickedPivot = nullptr;

    return true;
}

void
UOdysseyPainterEditorVectorTransformTool::Commit()
{

}

void
UOdysseyPainterEditorVectorTransformTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , const FName& iPropertyName )
{
    //mTransformHUD->MakeTransform( iScene, DivisionsX, DivisionsY );
}

#undef LOCTEXT_NAMESPACE
