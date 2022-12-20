// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorScaleObjectTool/OdysseyPainterEditorVectorScaleObjectTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorScaleObjectTool::~UOdysseyPainterEditorVectorScaleObjectTool()
{
}

UOdysseyPainterEditorVectorScaleObjectTool::UOdysseyPainterEditorVectorScaleObjectTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ScaleObjectTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorScaleObjectTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
}

bool
UOdysseyPainterEditorVectorScaleObjectTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorScaleObjectTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorObject* selectedObject = vectorEngine->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            mOldLocalMouseX = iPointInTexture.x;
            mOldLocalMouseY = iPointInTexture.y;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorScaleObjectTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorObject* selectedObject = vectorEngine->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            double difx = iPointInTexture.x - mOldLocalMouseX;
            double dify = iPointInTexture.y - mOldLocalMouseX;
            ::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );

            selectedObject->Scale( selectedObject->GetScalingX() + difx * 0.01f
                                 , selectedObject->GetScalingY() + dify * 0.01f );

            selectedObject->UpdateMatrix();

            mOldLocalMouseX = iPointInTexture.x;
            mOldLocalMouseY = iPointInTexture.y;

            RedrawCurrentLayer( { beforeBBox | selectedObject->GetBBox( true ) } );
        }
    }
}

bool
UOdysseyPainterEditorVectorScaleObjectTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorScaleObjectTool::Commit()
{

}
