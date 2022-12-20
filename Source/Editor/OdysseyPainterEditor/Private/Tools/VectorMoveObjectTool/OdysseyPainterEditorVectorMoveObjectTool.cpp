// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorMoveObjectTool/OdysseyPainterEditorVectorMoveObjectTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorMoveObjectTool::~UOdysseyPainterEditorVectorMoveObjectTool()
{
}

UOdysseyPainterEditorVectorMoveObjectTool::UOdysseyPainterEditorVectorMoveObjectTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.MoveObjectTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorMoveObjectTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
}

bool
UOdysseyPainterEditorVectorMoveObjectTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorMoveObjectTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorObject* selectedObject = vectorEngine->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            UOdysseyVectorObject* parentObject = selectedObject->GetParent();
            BLPoint localCoords = parentObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorMoveObjectTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetVectorEngine()->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            UOdysseyVectorObject* parentObject = selectedObject->GetParent();
            BLPoint localCoords = parentObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
            double difx = localCoords.x - mOldLocalMouseX;
            double dify = localCoords.y - mOldLocalMouseY;
            ::ULIS::FRectD beforeBBox = selectedObject->GetBBox( true );

            selectedObject->Translate( selectedObject->GetTranslationX() + difx
                                     , selectedObject->GetTranslationY() + dify );

            selectedObject->UpdateMatrix();

            mOldLocalMouseX = localCoords.x;
            mOldLocalMouseY = localCoords.y;

            RedrawCurrentLayer( { beforeBBox | selectedObject->GetBBox( true ) } );
        }
    }
}

bool
UOdysseyPainterEditorVectorMoveObjectTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorVectorMoveObjectTool::Commit()
{

}
