// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPrimitiveDrawingTool/OdysseyTextureEditorVectorPrimitiveDrawingTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorPrimitiveDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorPrimitiveDrawingTool::~UOdysseyTextureEditorVectorPrimitiveDrawingTool()
{
}

UOdysseyTextureEditorVectorPrimitiveDrawingTool::UOdysseyTextureEditorVectorPrimitiveDrawingTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorPrimitiveDrawingTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorVectorPrimitiveDrawingTool::OnCurrentLayerChanged);
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorVectorPrimitiveDrawingTool::Load()
{
	//Define the new active tool based on the layer type
	class UOdysseyTextureLayerImageVector* layer = GetLayer();
	if (!layer)
		return;

}

void
UOdysseyTextureEditorVectorPrimitiveDrawingTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

}

void
UOdysseyTextureEditorVectorPrimitiveDrawingTool::Unload()
{

}

bool
UOdysseyTextureEditorVectorPrimitiveDrawingTool::IsActivable() const
{
	if (!Super::IsActivable())
        return false; 

	//Check for currentlayer
	UOdysseyTextureLayerImageVector* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return isActive && !isLocked;
}

bool
UOdysseyTextureEditorVectorPrimitiveDrawingTool::CanDraw()
{	
    if (!Super::CanDraw())
        return false; 

	UOdysseyTextureLayerImageVector* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return !isActive || isLocked;
}

void
UOdysseyTextureEditorVectorPrimitiveDrawingTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	//ensure iLayerstack is the one the tool is working on
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !iLayerStack || !layerstack || layerstack != iLayerStack )
		return;

	//If not activable => Inactivate
	if (!IsActivable())
	{
		Inactivate(); //close the tool
		return;
	}

	//Reload the tool to edit the new layer
	Unload();
	Load();
}

UOdysseyTextureLayerImageVector*
UOdysseyTextureEditorVectorPrimitiveDrawingTool::GetLayer() const
{
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;

	return Cast<UOdysseyTextureLayerImageVector>(layerstack->CurrentLayer.Get());
}

#undef LOCTEXT_NAMESPACE
