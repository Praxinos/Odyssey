// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathDrawingTool/OdysseyAnimationEditorVectorPathDrawingTool.h"
#include "LayerStack/OdysseyAnimationLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorVectorPathDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyAnimationEditorVectorPathDrawingTool::~UOdysseyAnimationEditorVectorPathDrawingTool()
{
}

UOdysseyAnimationEditorVectorPathDrawingTool::UOdysseyAnimationEditorVectorPathDrawingTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyAnimationEditorVectorPathDrawingTool::Activate()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyAnimationLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyAnimationEditorVectorPathDrawingTool::OnCurrentLayerChanged );

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
UOdysseyAnimationEditorVectorPathDrawingTool::Load()
{
}

void
UOdysseyAnimationEditorVectorPathDrawingTool::Inactivate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyAnimationEditorVectorPathDrawingTool::Unload()
{
}

bool
UOdysseyAnimationEditorVectorPathDrawingTool::IsActivable() const
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
UOdysseyAnimationEditorVectorPathDrawingTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyAnimationEditorVectorPathDrawingTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
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
UOdysseyAnimationEditorVectorPathDrawingTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorPathDrawingTool::OnMouseHover( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

void
UOdysseyAnimationEditorVectorPathDrawingTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    UOdysseyAnimation* animation = layerStack->GetAnimation();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        ::ULIS::FRectI redrawRect;

        redrawRect = UOdysseyPainterEditorVectorPathDrawingTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        redrawRect = redrawRect & ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());

        currentVectorLayer->RenderImageChanged( { redrawRect }, true);
    }
}

bool
UOdysseyAnimationEditorVectorPathDrawingTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorPathDrawingTool::OnMouseUp( vectorEngine, vectorScene, nullptr, iPointInTexture, iKey );

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyAnimationEditorVectorPathDrawingTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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
