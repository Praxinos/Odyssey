// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectScaleTool::~UOdysseyPainterEditorVectorObjectScaleTool()
{
}

UOdysseyPainterEditorVectorObjectScaleTool::UOdysseyPainterEditorVectorObjectScaleTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectScaleTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectScaleTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

    vectorEngine->SetDrawingFlags( FOdysseyVectorEngine::RENDER_OBJECT_BBOX );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
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
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mPickedHandle = selectedObject->PickBBox( localCoords.x, localCoords.y );

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
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetScene()->GetLastSelected();

        if ( selectedObject && ( mPickedHandle > -1 ) )
        {
            BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            double difx = localCoords.x - mOldLocalMouseX;
            double dify = localCoords.y - mOldLocalMouseY;
            ::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );
            ::ULIS::FRectD localBBox = selectedObject->GetBBox( false );
            double oldX1 = localBBox.x
                 , oldY1 = localBBox.y
                 , oldX2 = localBBox.x + localBBox.w
                 , oldY2 = localBBox.y + localBBox.h;
            double x1, y1, x2, y2;
            ::ULIS::FVec2D pivot;
            BLPoint oldWorldPivot;
            BLPoint oldRelativePivot;
            BLPoint newWorldPivot;
            BLPoint newRelativePivot;

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

            oldWorldPivot = selectedObject->GetWorldMatrix().mapPoint( pivot.x, pivot.y );
            oldRelativePivot = selectedObject->GetParent()->GetInverseWorldMatrix().mapPoint( oldWorldPivot.x, oldWorldPivot.y );

            selectedObject->Scale( selectedObject->GetScalingX() * ((( x2 - x1 ) / fabs ( localBBox.w )))
                                 , selectedObject->GetScalingY() * ((( y2 - y1 ) / fabs ( localBBox.h ))) );
            selectedObject->UpdateMatrix();

            newWorldPivot = selectedObject->GetWorldMatrix().mapPoint( pivot.x, pivot.y );
            newRelativePivot = selectedObject->GetParent()->GetInverseWorldMatrix().mapPoint( newWorldPivot.x, newWorldPivot.y );
/*
UE_LOG(LogTemp, Warning, TEXT("Your message %f %f"), newWorldPivot.x, newWorldPivot.y );
*/
            selectedObject->Translate( selectedObject->GetTranslationX() + ( oldRelativePivot.x - newRelativePivot.x )
                                     , selectedObject->GetTranslationY() + ( oldRelativePivot.y - newRelativePivot.y ) );

            selectedObject->UpdateMatrix();

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            RedrawCurrentLayer( { /*beforeBBox | selectedObject->GetBBox( true )*/{ 0, 0, 0, 0 } } );
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

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorObjectScaleTool::Commit()
{

}
