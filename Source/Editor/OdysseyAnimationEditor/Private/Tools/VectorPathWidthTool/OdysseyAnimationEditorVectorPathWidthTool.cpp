// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathWidthTool/OdysseyAnimationEditorVectorPathWidthTool.h"
#include "LayerStack/OdysseyAnimationLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorVectorPathWidthTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyAnimationEditorVectorPathWidthTool::~UOdysseyAnimationEditorVectorPathWidthTool()
{
}

UOdysseyAnimationEditorVectorPathWidthTool::UOdysseyAnimationEditorVectorPathWidthTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyAnimationEditorVectorPathWidthTool::Activate()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyAnimationLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyAnimationEditorVectorPathWidthTool::OnCurrentLayerChanged );

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorPathWidthTool::Activate( vectorEngine, vectorScene );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyAnimationEditorVectorPathWidthTool::Load()
{
}

void
UOdysseyAnimationEditorVectorPathWidthTool::Inactivate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyAnimationEditorVectorPathWidthTool::Unload()
{
}

bool
UOdysseyAnimationEditorVectorPathWidthTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return currentLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass();
}

void
UOdysseyAnimationEditorVectorPathWidthTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyAnimationEditorVectorPathWidthTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorPathWidthTool::OnMouseDown( vectorEngine, vectorScene, iPointInTexture,iKey  );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyAnimationEditorVectorPathWidthTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorPathWidthTool::OnMouseHover( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

void
UOdysseyAnimationEditorVectorPathWidthTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorPathWidthTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyAnimationEditorVectorPathWidthTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorPathWidthTool::OnMouseUp( vectorEngine, vectorScene, iPointInTexture, iKey );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyAnimationEditorVectorPathWidthTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    //Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());

    // redraw
    if( currentVectorLayer )
    {
        currentVectorLayer->RenderImageChanged(false);
    }
}

#undef LOCTEXT_NAMESPACE
