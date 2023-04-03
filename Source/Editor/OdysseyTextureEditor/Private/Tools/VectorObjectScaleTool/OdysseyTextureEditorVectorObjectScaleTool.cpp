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

	//Reload the tool to edit the new layer
	Unload();
	Load();
}

bool
UOdysseyTextureEditorVectorObjectScaleTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = GetEditorAs<FOdysseyTextureEditor>()->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ObjectScaleTool", "Scale object"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        ret = UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDown( vectorEngine, vectorScene, &undo, iPointInTexture,iKey  );

        if( undo )
        {
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
            undo->mRefreshDelegate.AddUObject( this, &UOdysseyPainterEditorVectorObjectScaleTool::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    GEditor->EndTransaction();

    return ret;
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
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorVectorObjectScaleTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);

        // Update the VectorObjectTab widget
        vectorObjectTab.Get()->Update( vectorScene );
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
