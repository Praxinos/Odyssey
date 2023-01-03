// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyTextureEditorVectorPathCutTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorPathCutTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorPathCutTool::~UOdysseyTextureEditorVectorPathCutTool()
{
}

UOdysseyTextureEditorVectorPathCutTool::UOdysseyTextureEditorVectorPathCutTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorPathCutTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorVectorPathCutTool::OnCurrentLayerChanged);
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorVectorPathCutTool::Load()
{
	//Define the new active tool based on the layer type
	class UOdysseyTextureLayerImageVector* layer = GetLayer();
	if (!layer)
		return;

}

void
UOdysseyTextureEditorVectorPathCutTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

}

void
UOdysseyTextureEditorVectorPathCutTool::Unload()
{

}

bool
UOdysseyTextureEditorVectorPathCutTool::IsActivable() const
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
UOdysseyTextureEditorVectorPathCutTool::CanDraw()
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
UOdysseyTextureEditorVectorPathCutTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorPathCutTool::GetLayer() const
{
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;

	return Cast<UOdysseyTextureLayerImageVector>(layerstack->CurrentLayer.Get());
}

#undef LOCTEXT_NAMESPACE
