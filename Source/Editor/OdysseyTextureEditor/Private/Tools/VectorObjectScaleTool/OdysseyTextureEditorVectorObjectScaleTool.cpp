// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectScaleTool/OdysseyTextureEditorVectorObjectScaleTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorObjectScaleTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorObjectScaleTool::~UOdysseyTextureEditorVectorObjectScaleTool()
{
}

UOdysseyTextureEditorVectorObjectScaleTool::UOdysseyTextureEditorVectorObjectScaleTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorObjectScaleTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorVectorObjectScaleTool::OnCurrentLayerChanged);
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorVectorObjectScaleTool::Load()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorObjectScaleTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyTextureEditorVectorObjectScaleTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

    Unload();
}

void
UOdysseyTextureEditorVectorObjectScaleTool::Unload()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

        if( currentVectorLayer )
        {
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

            UOdysseyPainterEditorVectorObjectScaleTool::UnloadVector( vectorEngine, vectorScene );
        }
    }
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass();
}

void
UOdysseyTextureEditorVectorObjectScaleTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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

    // We have to redraw all layers in order to draw all layers without the HUD.
    // This will be removed when we'll have a dedicated HUD layer.
    TArray<UOdysseyLayer*> layers = iLayerStack->GetLayers();
    for( int i = 0; i < layers.Num(); i++ )
    {
        UOdysseyTextureLayer* textureLayer = static_cast<UOdysseyTextureLayer*>(layers[i]);
        textureLayer->RenderImageChanged(false);
    }

	//Reload the tool to edit the new layer
	Unload();
	Load();
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::OnKeyDown( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::OnKeyUp( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    }

    return ret;
}

void
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorObjectScaleTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

#undef LOCTEXT_NAMESPACE
