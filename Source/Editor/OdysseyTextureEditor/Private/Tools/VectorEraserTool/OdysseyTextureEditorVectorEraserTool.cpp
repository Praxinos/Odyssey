// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyTextureEditorVectorEraserTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorEraserTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorEraserTool::~UOdysseyTextureEditorVectorEraserTool()
{
}

UOdysseyTextureEditorVectorEraserTool::UOdysseyTextureEditorVectorEraserTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorEraserTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorVectorEraserTool::OnCurrentLayerChanged );

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorEraserTool::Activate( vectorEngine, vectorScene );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyTextureEditorVectorEraserTool::Load()
{

}

void
UOdysseyTextureEditorVectorEraserTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyTextureEditorVectorEraserTool::Unload()
{
}

bool
UOdysseyTextureEditorVectorEraserTool::IsActivable() const
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
UOdysseyTextureEditorVectorEraserTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorEraserTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                  , const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyTextureEditorVectorEraserTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorEraserTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged( true );
    }
}

void
UOdysseyTextureEditorVectorEraserTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyTextureEditorVectorEraserTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("EraserTool", "Erase"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        ret = UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( vectorEngine, vectorScene, &undo, iPointInTexture, iKey );

        if( undo )
        {
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    GEditor->EndTransaction();

    return ret;
}

void
UOdysseyTextureEditorVectorEraserTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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
