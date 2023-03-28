// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyTextureEditorVectorPathDrawingTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorPathDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorPathDrawingTool::~UOdysseyTextureEditorVectorPathDrawingTool()
{
}

UOdysseyTextureEditorVectorPathDrawingTool::UOdysseyTextureEditorVectorPathDrawingTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorPathDrawingTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorVectorPathDrawingTool::OnCurrentLayerChanged );

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorPathDrawingTool::Activate( vectorEngine, vectorScene );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyTextureEditorVectorPathDrawingTool::Load()
{
}

void
UOdysseyTextureEditorVectorPathDrawingTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyTextureEditorVectorPathDrawingTool::Unload()
{
}

bool
UOdysseyTextureEditorVectorPathDrawingTool::IsActivable() const
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
UOdysseyTextureEditorVectorPathDrawingTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorPathDrawingTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDown( vectorEngine, vectorScene, iPointInTexture,iKey  );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyTextureEditorVectorPathDrawingTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

void
UOdysseyTextureEditorVectorPathDrawingTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        ::ULIS::FRectI redrawRect;

        redrawRect = UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        redrawRect = redrawRect & layerStack->GetSurface()->Block()->Rect();

        currentVectorLayer->RenderImageChanged( { redrawRect }, true);
    }
}

bool
UOdysseyTextureEditorVectorPathDrawingTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp( vectorEngine, vectorScene, iPointInTexture, iKey );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyTextureEditorVectorPathDrawingTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

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
