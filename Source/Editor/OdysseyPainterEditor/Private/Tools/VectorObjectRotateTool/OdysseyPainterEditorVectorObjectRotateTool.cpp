// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

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

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectRotateTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox(); // this is a reference, it does not matter it we get it before the call to UpdateSelectionBox 

        mTransformHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );

        mTransformHUD->SetPivot( selectionBox.rect.x + ( selectionBox.rect.w * 0.5f )
                               , selectionBox.rect.y + ( selectionBox.rect.h * 0.5f ) );

        vectorEngine->ClearHUD( );
        vectorEngine->AddHUD( mTransformHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mPickedPivot = mTransformHUD->PickPivot( iPointInTexture.x, iPointInTexture.y ) ? &mTransformHUD->GetPivot() : nullptr;

        mTransformHUD->SetShowBox( false );
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

        if ( mPickedPivot )
        {
            BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mTransformHUD->SetPivot( localCoords.x, localCoords.y );
        }
        else
        {
            std::list<UOdysseyVectorObject*>& selectedObjectList = currentVectorLayer->GetScene()->GetSelectedObjectList();
            //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
            //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
            BLMatrix2D spaceMatrix = selectionBox.space->GetWorldMatrix();
            BLMatrix2D invertSpaceMatrix;
            ::ULIS::FVec2D& pivot = mTransformHUD->GetPivot();

            spaceMatrix.translate( pivot.x, pivot.y );

            BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

            for( std::list<UOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                UOdysseyVectorObject* object = (*it);
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
            currentVectorLayer->GetScene()->UpdateMatrix();

            // update the selection box with the newly modified matrices
            mTransformHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );
        }

        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        mTransformHUD->SetShowBox( true );

        currentVectorLayer->RenderImageChanged( true );

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorObjectRotateTool::Commit()
{

}
