// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorRotateObjectTool/OdysseyPainterEditorVectorRotateObjectTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorRotateObjectTool::~UOdysseyPainterEditorVectorRotateObjectTool()
{
}

UOdysseyPainterEditorVectorRotateObjectTool::UOdysseyPainterEditorVectorRotateObjectTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.RotateObjectTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorRotateObjectTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();

    vectorEngine->SetDrawingFlags( 0 );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
}

bool
UOdysseyPainterEditorVectorRotateObjectTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorRotateObjectTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorVectorRotateObjectTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
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
UOdysseyPainterEditorVectorRotateObjectTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorVectorRotateObjectTool::Commit()
{

}
