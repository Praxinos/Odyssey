// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateTool.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectRotateTool::~UOdysseyPainterEditorVectorObjectRotateTool()
{
    delete mTransformHUD;
}

UOdysseyPainterEditorVectorObjectRotateTool::UOdysseyPainterEditorVectorObjectRotateTool()
    : mPickedPivot( nullptr )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectRotateTool64");

    mTransformHUD = new FOdysseyVectorHUDRotate();
}

// refresh on Undo for example (we need to reset the selection box as if the tool was activated)
void
UOdysseyPainterEditorVectorObjectRotateTool::OnRefresh( FOdysseyVectorScene* iScene )
{
    FitHUD( iScene );
}

void
UOdysseyPainterEditorVectorObjectRotateTool::FitHUD( FOdysseyVectorScene* iScene )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox(); // this is a reference, it does not matter it we get it before the call to UpdateSelectionBox 

    mTransformHUD->UpdateSelectionBox( iScene );

    mTransformHUD->SetPivot( selectionBox.rect.x + ( selectionBox.rect.w * 0.5f )
                           , selectionBox.rect.y + ( selectionBox.rect.h * 0.5f ) );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectRotateTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    FitHUD( iScene );

    iEngine->ClearHUD( );
    iEngine->AddHUD( mTransformHUD );
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , FOdysseyVectorUndo** iUndo
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    mPickedPivot = mTransformHUD->PickPivot( iPointInTexture.x, iPointInTexture.y ) ? &mTransformHUD->GetPivot() : nullptr;

    mTransformHUD->SetShowBox( false );

    // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
    if( iUndo && GUndo )
    {
        // save selected object translation/rotation/scaling before transform
        (*iUndo) = new FOdysseyVectorUndoObjectTransform( iScene, iScene->GetSelectedObjectList() );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

    if( selectionBox.space )
    {
        if ( mPickedPivot )
        {
            BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mTransformHUD->SetPivot( localCoords.x, localCoords.y );
        }
        else
        {
            std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
            //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
            //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
            BLMatrix2D spaceMatrix = selectionBox.space->GetWorldMatrix();
            BLMatrix2D invertSpaceMatrix;
            ::ULIS::FVec2D& pivot = mTransformHUD->GetPivot();

            spaceMatrix.translate( pivot.x, pivot.y );

            BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

            for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                FOdysseyVectorObject* object = (*it);
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
                BLMatrix2D rotateMatrix;

                // transfer object in "Rotation Space" coordinates system
                FOdysseyVector::MatrixMultiply( invertSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

                rotateMatrix.reset();
                rotateMatrix.rotate( iPointInTexture.deltaPosition.X * 0.01f ); // Radians

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
            }

            // Update the matrix for all objects
            iScene->UpdateMatrix();

            iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );

            // update the selection box with the newly modified matrices
            mTransformHUD->UpdateSelectionBox( iScene );
        }
    }
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    mTransformHUD->SetShowBox( true );

    iScene->Update( 0 );

    return true;
}

void
UOdysseyPainterEditorVectorObjectRotateTool::Commit()
{

}
