// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectScaleTool::~UOdysseyPainterEditorVectorObjectScaleTool()
{
    delete mTransformHUD;
}

UOdysseyPainterEditorVectorObjectScaleTool::UOdysseyPainterEditorVectorObjectScaleTool()
    : Uniform( true )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectScaleTool64");

    mTransformHUD = new FOdysseyVectorHUDScale( );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectScaleTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mTransformHUD->UpdateSelectionBox( currentVectorLayer->GetScene() );

        vectorEngine->ClearHUD( );
        vectorEngine->AddHUD( mTransformHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorObjectScaleTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

        if( selectionBox.space )
        {
            BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mPickedHandle = mTransformHUD->Pick( iPointInTexture.x, iPointInTexture.y );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

        if ( selectionBox.space )
        {
            BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            std::list<FOdysseyVectorObject*>& selectedObjectList = currentVectorLayer->GetScene()->GetSelectedObjectList();
            double difx = localCoords.x - mOldLocalMouseX;
            double dify = localCoords.y - mOldLocalMouseY;
            //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
            //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
            double oldX1 = selectionBox.rect.x
                 , oldY1 = selectionBox.rect.y
                 , oldX2 = selectionBox.rect.x + selectionBox.rect.w
                 , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
            double oldDiagonal = sqrt( ( selectionBox.rect.w * selectionBox.rect.w )
                                     + ( selectionBox.rect.h * selectionBox.rect.h ) );
            double x1, y1, x2, y2;
            ::ULIS::FVec2D pivot;

            if ( mPickedHandle == 0 )
            {
                x1 = localCoords.x;
                y1 = localCoords.y;
                x2 = oldX2;
                y2 = oldY2;

                pivot.x = oldX2;
                pivot.y = oldY2;
            }

            if ( mPickedHandle == 1 )
            {
                x1 = oldX1;
                y1 = localCoords.y;
                x2 = localCoords.x;
                y2 = oldY2;

                pivot.x = oldX1;
                pivot.y = oldY2;
            }

            if ( mPickedHandle == 2 )
            {
                x1 = oldX1;
                y1 = oldY1;
                x2 = localCoords.x;
                y2 = localCoords.y;

                pivot.x = oldX1;
                pivot.y = oldY1;
            }

            if ( mPickedHandle == 3 )
            {
                x1 = localCoords.x;
                y1 = oldY1;
                x2 = oldX2;
                y2 = localCoords.y;

                pivot.x = oldX2;
                pivot.y = oldY1;
            }

            if( mPickedHandle != -1 )
            {
                BLMatrix2D spaceMatrix = selectionBox.space->GetWorldMatrix();
                BLMatrix2D invertSpaceMatrix;
                double x2mx1 = ( x2 - x1 );
                double y2my1 = ( y2 - y1 );
                BLMatrix2D scalingMatrix;

                spaceMatrix.translate( pivot.x, pivot.y );

                BLMatrix2D::invert( invertSpaceMatrix, spaceMatrix );

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

                    // transfer object in "Scaling Space" coordinates system
                    FOdysseyVector::MatrixMultiply( invertSpaceMatrix, objectWorldMatrix, objectSpaceMatrix );

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
                    object->Rotate( rotation / M_PI * 180 );
                    object->Scale( scalingX, scalingY );
                }
            }

            // Update the matrix for all objects
            currentVectorLayer->GetScene()->UpdateMatrix();

            // update the selection box with the newly modified matrices
            mTransformHUD->UpdateSelectionBox( currentVectorLayer->GetScene() );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            currentVectorLayer->RenderImageChanged(true);
            //RedrawCurrentLayer( { /*beforeBBox | selectedObject->GetBBox( true )*/{ 0, 0, 0, 0 } } );
        }
    }
}

bool
UOdysseyPainterEditorVectorObjectScaleTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorObjectScaleTool::Commit()
{

}
