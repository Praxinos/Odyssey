// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyTextureEditorVectorObjectPickTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorObjectPickTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorObjectPickTool::~UOdysseyTextureEditorVectorObjectPickTool()
{
}

UOdysseyTextureEditorVectorObjectPickTool::UOdysseyTextureEditorVectorObjectPickTool()
{
}

// refresh on Undo for example (we need to reset the selection box as if the tool was activated)
void
UOdysseyTextureEditorVectorObjectPickTool::OnRefresh( FOdysseyVectorScene* iScene )
{
    Activate();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorObjectPickTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    UTexture2D* texture = layerStack->GetTexture();

    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorVectorObjectPickTool::OnCurrentLayerChanged );

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorObjectPickTool::Activate( vectorEngine, vectorScene, texture->Source.GetSizeX(), texture->Source.GetSizeY() );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyTextureEditorVectorObjectPickTool::Load()
{
}

void
UOdysseyTextureEditorVectorObjectPickTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyTextureEditorVectorObjectPickTool::Unload()
{
}

bool
UOdysseyTextureEditorVectorObjectPickTool::IsActivable() const
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
UOdysseyTextureEditorVectorObjectPickTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorObjectPickTool::OnMouseDown(const FOdysseyPoint& iPointInTexture,const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorVectorObjectPickTool::OnMouseDown(vectorEngine,vectorScene,iPointInTexture,iKey);

        currentVectorLayer->RenderImageChanged(false);
    }

    return ret;
}

void
UOdysseyTextureEditorVectorObjectPickTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorObjectPickTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyTextureEditorVectorObjectPickTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ObjectPickTool", "Pick object"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        ret = UOdysseyPainterEditorVectorObjectPickTool::OnMouseUp( vectorEngine, vectorScene, &undo, iPointInTexture, iKey );

        if( undo )
        {
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
            undo->mRefreshDelegate.AddUObject( this, &UOdysseyTextureEditorVectorObjectPickTool::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged(false);

        // Update the VectorObjectTab widget
        vectorObjectTab.Get()->Update( vectorScene );
    }

    GEditor->EndTransaction();

    return ret;
}

#undef LOCTEXT_NAMESPACE
