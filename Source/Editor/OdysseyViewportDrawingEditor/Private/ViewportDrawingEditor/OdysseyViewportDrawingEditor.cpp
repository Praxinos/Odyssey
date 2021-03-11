// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditor.h"

#include "OdysseyViewportDrawingEditorGUI.h"


#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditor"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditor::~FOdysseyViewportDrawingEditor()
{
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

FOdysseyViewportDrawingEditor::FOdysseyViewportDrawingEditor() :
	FOdysseyTextureEditor(),
	mGUI(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditor::InitData()
{
	FOdysseyTextureEditor::InitData();

	//Handle Object Property Changed Callback to refresh when actors's visibility changes for example
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this,&FOdysseyViewportDrawingEditor::OnObjectPropertyChanged);

	//--- Init Data
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

AActor*
FOdysseyViewportDrawingEditor::Actor() const
{
    return mActor;
}

UMeshComponent*
FOdysseyViewportDrawingEditor::Component() const
{
    return mComponent;
}

const TArray<UMeshComponent*>&
FOdysseyViewportDrawingEditor::SelectableComponents() const
{
    return mSelectableComponents;
}

const TArray<FPaintableTexture>&
FOdysseyViewportDrawingEditor::SelectableTextures() const
{
    return mSelectableTextures;
}

const TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>&
FOdysseyViewportDrawingEditor::ComponentToAdapterMap() const
{
	return mComponentToAdapterMap;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyViewportDrawingEditor::Actor(AActor* iActor)
{
	if (mActor == iActor)
		return;

	// Clear everything before changing actor
	ClearSelectableComponents(); //also clear selected component / texture and selectable textures

	// Change the selected Actor
	mActor = iActor;

	// Refresh Selectable Components/Textures if needed
	if (mActor)
	{
		UpdateSelectableComponents();
		SelectDefaultComponent(); //Also Selects the default texture if needed
	}
}

void
FOdysseyViewportDrawingEditor::Component(UMeshComponent* iComponent)
{
	if (mComponent == iComponent)
		return;

	// Save Component Paint Settings
	if (mComponent)
	{
		FInstanceTexturePaintSettings& texturePaintSettings = mComponentToTexturePaintSettingsMap.FindOrAdd(mComponent);
		texturePaintSettings.mSelectedTexture = Texture();
	}

	// Clear everything before changing component
	ClearSelectableTextures(); // Also clears selected Texture

	// Change the selected component
	mComponent = iComponent;

	if (mComponent)
	{
		UpdateSelectableTextures();
		SelectDefaultTexture();
	}
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyViewportDrawingEditorGUI*
FOdysseyViewportDrawingEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyViewportDrawingEditorGUI(this));
	return mGUI.Get();
}

void
FOdysseyViewportDrawingEditor::OnPreTextureChange(UTexture2D* iNewTexture)
{
	UTexture2D* texture = Texture();
    if (texture)
	{
		RemoveEditedObject(texture);
	}

	FOdysseyTextureEditor::OnPreTextureChange(iNewTexture);
}

void
FOdysseyViewportDrawingEditor::OnPostTextureChange(UTexture2D* iOldTexture)
{
	UTexture2D* texture = Texture();

    if (texture)
	{
		AddEditedObject(texture);
	}

    FOdysseyTextureEditor::OnPostTextureChange(iOldTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Private Methods

void
FOdysseyViewportDrawingEditor::OnObjectPropertyChanged(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent)
{
	// For now we do not know how to manage actors/components visibility in the viewport
	// This method is called when we change the "Rendering>Visible" property of an actor/component
	// But it is not called when just hidding the actor from the world outliner and we don't know how to detect that properly
	// So for now, a hidden actor/component can still be selected for texture edition
	// Having nothing shown in the viewport in that case makes it viable

    //TODO: Get the visibility value
	//AActor* actor = Cast<AActor>(iObject);
	/*if ( actor == mActor &&
        iPropertyChangedEvent.Property && 
		iPropertyChangedEvent.Property->GetName() == USceneComponent::GetVisiblePropertyName().ToString())
	{
		Refresh();
	}*/
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Private Methods

void
FOdysseyViewportDrawingEditor::ClearSelectableComponents()
{
	Component(nullptr);
	mSelectableComponents.Empty();
	for (auto meshAdapterPair : mComponentToAdapterMap)
	{
		meshAdapterPair.Value->OnRemoved();
	}
	mComponentToAdapterMap.Empty();
	FMeshPaintAdapterFactory::CleanupGlobals();
}

void
FOdysseyViewportDrawingEditor::UpdateSelectableComponents()
{
	//make sure the array in empty
	ClearSelectableComponents();

	if (!mActor)
		return;

	// Get Actor's components
	TArray<UMeshComponent*> actorComponents;
	TInlineComponentArray<UMeshComponent*> inlineActorComponents;
	mActor->GetComponents(actorComponents);
	for (UMeshComponent* Component : actorComponents)
	{
		actorComponents.AddUnique(Component); // Make sur to have unique components in the array
	}

	// Fill mSelectable Components with visible components and prepare mesh for texture edition
	TUniquePtr< FComponentReregisterContext > ComponentReregisterContext; //ES: I don't know what this is
	for (UMeshComponent* meshComponent : actorComponents)
	{
		TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = FMeshPaintAdapterFactory::CreateAdapterForMesh(meshComponent, 0); //ES: I don't know what this is
		if (meshComponent->IsVisible() && meshAdapter.IsValid() && meshAdapter->IsValid())
		{
			mSelectableComponents.Add(meshComponent);
			mComponentToAdapterMap.Add(meshComponent, meshAdapter);
			meshAdapter->OnAdded();
			MeshPaintHelpers::ForceRenderMeshLOD(meshComponent, 0);
			ComponentReregisterContext.Reset(new FComponentReregisterContext(meshComponent)); //ES: I don't know what this does
		}
	}
}

void
FOdysseyViewportDrawingEditor::SelectDefaultComponent()
{
	//TODO: make it smarter, like SelectDefaultTexture does
	// for now we only select the first component of the selected actor

	if (!mActor)
		return;

	if (mSelectableComponents.Num() <= 0)
		return;

	Component(mSelectableComponents[0]);
}

void
FOdysseyViewportDrawingEditor::ClearSelectableTextures()
{
	Texture(nullptr);
	mSelectableTextures.Empty();
}

void
FOdysseyViewportDrawingEditor::UpdateSelectableTextures()
{
	//make sure the array in empty
	ClearSelectableTextures();

	if (!mComponent)
		return;

	TSharedPtr<IMeshPaintGeometryAdapter> adapter = mComponentToAdapterMap.FindChecked(mComponent);
	TexturePaintHelpers::RetrieveTexturesForComponent(mComponent, adapter.Get(), mSelectableTextures);
}

void
FOdysseyViewportDrawingEditor::SelectDefaultTexture()
{
	if (!mComponent)
		return;

	if (mSelectableTextures.Num() <= 0)
		return;
		
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	bool displayWarning = true;

	//try to select the previously selected texture for the selected component
	FInstanceTexturePaintSettings& texturePaintSettings = mComponentToTexturePaintSettingsMap.FindOrAdd(mComponent);
	if (texturePaintSettings.mSelectedTexture && mSelectableTextures.Contains(texturePaintSettings.mSelectedTexture))
	{
		if (texturePaintSettings.mSelectedTexture == Texture()) //if the texture is already selected we assume we have nothing to do
			return;

		if (AssetEditorSubsystem->FindEditorForAsset(texturePaintSettings.mSelectedTexture, true) == nullptr)
		{
			Texture(texturePaintSettings.mSelectedTexture);
			return;
		}
		
		if (displayWarning) //only display the Warning Message for the first texture
		{
			FText Title = LOCTEXT("TitleSelectedtextureAlreadyOpened", "Selected Texture Already Opened");
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedtextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);
			displayWarning = false;
		}
	}
	else
	{
		texturePaintSettings.mSelectedTexture = nullptr;
	}

	// select the first texture available for edition
	for (FPaintableTexture& paintableTexture : mSelectableTextures)
	{
		UTexture2D* texture = Cast<UTexture2D>(paintableTexture.Texture);
		if (texture == Texture()) //if the texture is already selected we assume we have nothing to do
			break;

		if (AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
		{
			if (displayWarning) //only display the Warning Message for the first texture
			{
				FText Title = LOCTEXT("TitleSelectedtextureAlreadyOpened", "Selected Texture Already Opened");
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SelectedtextureAlreadyOpened", "The selected texture is already opened in an other editor. Please close the editor before selecting this texture."), &Title);
				displayWarning = false;
			}
			continue;
		}

		Texture(texture);
		break;
	}	
}

#undef LOCTEXT_NAMESPACE