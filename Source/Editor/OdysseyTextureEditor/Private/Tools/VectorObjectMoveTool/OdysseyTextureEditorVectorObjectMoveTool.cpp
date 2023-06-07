// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectMoveTool/OdysseyTextureEditorVectorObjectMoveTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorObjectMoveTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorObjectMoveTool::~UOdysseyTextureEditorVectorObjectMoveTool()
{
}

UOdysseyTextureEditorVectorObjectMoveTool::UOdysseyTextureEditorVectorObjectMoveTool()
{
}

// refresh on Undo for example (we need to reset the selection box as if the tool was activated)
void
UOdysseyTextureEditorVectorObjectMoveTool::OnRefresh( FOdysseyVectorScene* iScene )
{
    Activate();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorObjectMoveTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorVectorObjectMoveTool::OnCurrentLayerChanged );
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorVectorObjectMoveTool::Load()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorObjectMoveTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyTextureEditorVectorObjectMoveTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

    Unload();
}

void
UOdysseyTextureEditorVectorObjectMoveTool::Unload()
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

            UOdysseyPainterEditorVectorObjectMoveTool::UnloadVector( vectorEngine, vectorScene );
        }
    }
}

bool
UOdysseyTextureEditorVectorObjectMoveTool::IsActivable() const
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
UOdysseyTextureEditorVectorObjectMoveTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorObjectMoveTool::OnKeyDown( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectMoveTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorVectorObjectMoveTool::OnKeyUp( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectMoveTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorVectorObjectMoveTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyTextureEditorVectorObjectMoveTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

bool
UOdysseyTextureEditorVectorObjectMoveTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorObjectMoveTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

#undef LOCTEXT_NAMESPACE
