// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSceneScaleTool/OdysseyTextureEditorVectorSceneScaleTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorSceneScaleTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorSceneScaleTool::~UOdysseyTextureEditorVectorSceneScaleTool()
{
}

UOdysseyTextureEditorVectorSceneScaleTool::UOdysseyTextureEditorVectorSceneScaleTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorSceneScaleTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorVectorSceneScaleTool::OnCurrentLayerChanged);
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorVectorSceneScaleTool::Load()
{
	//Define the new active tool based on the layer type
	class UOdysseyTextureLayerImageVector* layer = GetLayer();
	if (!layer)
		return;

}

void
UOdysseyTextureEditorVectorSceneScaleTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

}

void
UOdysseyTextureEditorVectorSceneScaleTool::Unload()
{

}

bool
UOdysseyTextureEditorVectorSceneScaleTool::IsActivable() const
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
UOdysseyTextureEditorVectorSceneScaleTool::CanDraw()
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
UOdysseyTextureEditorVectorSceneScaleTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorSceneScaleTool::GetLayer() const
{
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;

	return Cast<UOdysseyTextureLayerImageVector>(layerstack->CurrentLayer.Get());
}

#undef LOCTEXT_NAMESPACE
