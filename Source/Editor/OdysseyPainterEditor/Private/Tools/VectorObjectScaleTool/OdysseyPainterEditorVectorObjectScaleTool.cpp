// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVector.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectScaleTool::~UOdysseyPainterEditorVectorObjectScaleTool()
{
    delete mTransformHUD;
}

UOdysseyPainterEditorVectorObjectScaleTool::UOdysseyPainterEditorVectorObjectScaleTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectScaleTool64");

    mTransformHUD = new FOdysseyVectorHUDTransform( );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectScaleTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mTransformHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );

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
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
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
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mTransformHUD->GetSelectionBox();

        if ( selectionBox.space )
        {
            BLPoint localCoords = selectionBox.space->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            std::list<UOdysseyVectorObject*>& selectedObjectList = currentVectorLayer->GetScene()->GetSelectedObjectList();
            double difx = localCoords.x - mOldLocalMouseX;
            double dify = localCoords.y - mOldLocalMouseY;
            //::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
            //::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
            double oldX1 = selectionBox.rect.x
                 , oldY1 = selectionBox.rect.y
                 , oldX2 = selectionBox.rect.x + selectionBox.rect.w
                 , oldY2 = selectionBox.rect.y + selectionBox.rect.h;
            double x1, y1, x2, y2;
            ::ULIS::FVec2D pivot;
            ///BLPoint oldWorldPivot;
            //BLPoint oldRelativePivot;
            //BLPoint newWorldPivot;
            //BLPoint newRelativePivot;

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

            BLMatrix2D matrix;

            matrix.reset();
            matrix.scale( ( x2 - x1 ) / selectionBox.rect.w, ( y2 - y1 ) / selectionBox.rect.h );

            for( std::list<UOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
            {
                UOdysseyVectorObject* object = (*it);
                //double translationX;
                //double translationY;
               // double rotation;
                double scalingX;
                double scalingY;
                BLMatrix2D objectWorldMatrix = object->GetWorldMatrix();

                //objectWorldMatrix.transform( selectionBox.space->GetInverseWorldMatrix() );
                //objectWorldMatrix.transform( matrix );
                objectWorldMatrix.scale( ( x2 - x1 ) / selectionBox.rect.w, ( y2 - y1 ) / selectionBox.rect.h );
                objectWorldMatrix.transform( object->GetParent()->GetInverseWorldMatrix() );

                FOdysseyVector::ExtractTransformations( objectWorldMatrix
                                                      , nullptr//&translationX
                                                      , nullptr//&translationY
                                                      , nullptr//&rotation
                                                      , &scalingX
                                                      , &scalingY );
UE_LOG(LogTemp, Warning, TEXT("Some warning message x:%f =? %f - y:%f =? %f"), scalingX, ( x2 - x1 ) / selectionBox.rect.w,
                                                                               scalingY, ( y2 - y1 ) / selectionBox.rect.h ); 
                //object->Translate( translationX, translationY );
                //object->Rotate( rotation );
                object->Scale( scalingX, scalingY );
            }

            currentVectorLayer->GetScene()->UpdateMatrix();

            mTransformHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );
/*
            oldWorldPivot = selectedObject->GetWorldMatrix().mapPoint( pivot.x, pivot.y );
            oldRelativePivot = selectedObject->GetParent()->GetInverseWorldMatrix().mapPoint( oldWorldPivot.x, oldWorldPivot.y );

            selectedObject->Scale( selectedObject->GetScalingX() * ((( x2 - x1 ) / fabs ( localBBox.w )))
                                 , selectedObject->GetScalingY() * ((( y2 - y1 ) / fabs ( localBBox.h ))) );
            selectedObject->UpdateMatrix();

            newWorldPivot = selectedObject->GetWorldMatrix().mapPoint( pivot.x, pivot.y );
            newRelativePivot = selectedObject->GetParent()->GetInverseWorldMatrix().mapPoint( newWorldPivot.x, newWorldPivot.y );

            selectedObject->Translate( selectedObject->GetTranslationX() + ( oldRelativePivot.x - newRelativePivot.x )
                                     , selectedObject->GetTranslationY() + ( oldRelativePivot.y - newRelativePivot.y ) );

            selectedObject->UpdateMatrix();
*/


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
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        currentVectorLayer->RenderImageChanged(false);
        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorObjectScaleTool::Commit()
{

}
