// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectRotateTool::~UOdysseyPainterEditorVectorObjectRotateTool()
{
}

UOdysseyPainterEditorVectorObjectRotateTool::UOdysseyPainterEditorVectorObjectRotateTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectRotateTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectRotateTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();

    vectorEngine->SetDrawingFlags( 0 );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
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
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
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
            double dify = iPointInTexture.y - mOldLocalMouseY;
            ::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );

            selectedObject->Rotate(selectedObject->GetRotation() + ( difx + dify ) * 0.01f);

            selectedObject->UpdateMatrix();

            mOldLocalMouseX = iPointInTexture.x;
            mOldLocalMouseY = iPointInTexture.y;

            RedrawCurrentLayer( { beforeBBox | selectedObject->GetBBox( true ) } );
        }
    }
}

bool
UOdysseyPainterEditorVectorObjectRotateTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorVectorObjectRotateTool::Commit()
{

}
