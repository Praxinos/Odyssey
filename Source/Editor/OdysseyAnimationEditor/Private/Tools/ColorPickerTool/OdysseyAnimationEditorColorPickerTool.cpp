// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/ColorPickerTool/OdysseyAnimationEditorColorPickerTool.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorColorPickerTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyAnimationEditorColorPickerTool::~UOdysseyAnimationEditorColorPickerTool()
{
}

UOdysseyAnimationEditorColorPickerTool::UOdysseyAnimationEditorColorPickerTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyAnimationEditorColorPickerTool::Load()
{
}

void
UOdysseyAnimationEditorColorPickerTool::Activate()
{
    UOdysseyAnimationLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyAnimationEditorColorPickerTool::OnCurrentLayerChanged );
    Load();
    Super::Activate();
}

void
UOdysseyAnimationEditorColorPickerTool::Unload()
{
}

void
UOdysseyAnimationEditorColorPickerTool::Inactivate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
	Unload();
}

bool
UOdysseyAnimationEditorColorPickerTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return      /*currentLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass()
            ||  */currentLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass();
}

void
UOdysseyAnimationEditorColorPickerTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	//ensure iLayerstack is the one the tool is working on
	UOdysseyAnimationLayerStack* layerstack = GetEditorAs<FOdysseyAnimationEditor>()->LayerStack();
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

bool
UOdysseyAnimationEditorColorPickerTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorColorPickerTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

#undef LOCTEXT_NAMESPACE
