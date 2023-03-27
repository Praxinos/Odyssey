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
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorVectorObjectScaleTool::OnCurrentLayerChanged);

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorObjectScaleTool::Activate( vectorEngine, vectorScene );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyTextureEditorVectorObjectScaleTool::Load()
{
}

void
UOdysseyTextureEditorVectorObjectScaleTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyTextureEditorVectorObjectScaleTool::Unload()
{
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::IsActivable() const
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

	if (!Super::IsActivable())
        return false; 

	//Check for currentlayer
	if (!currentLayer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(currentLayer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(currentLayer);
    return isActive && !isLocked;
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

	//Reload the tool to edit the new layer
	Unload();
	Load();
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDown( vectorEngine, vectorScene, iPointInTexture,iKey  );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
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
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnMouseUp( vectorEngine, vectorScene, iPointInTexture, iKey );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

#undef LOCTEXT_NAMESPACE
