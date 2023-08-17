// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"

#include "PainterEditor/OdysseyPainterEditor.h"
#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorGUI.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorExtension"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyViewportDrawingEditorExtension::~FOdysseyViewportDrawingEditorExtension()
{
	int a = 0;
}

FOdysseyViewportDrawingEditorExtension::FOdysseyViewportDrawingEditorExtension()
	: FOdysseyPainterEditorExtension(nullptr)
	, mGUI(nullptr)
	, mPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased)
    , mActor(nullptr)
    , mComponent(nullptr)
    , mMaterial(nullptr)
{}

FOdysseyViewportDrawingEditorExtension::FOdysseyViewportDrawingEditorExtension(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorExtension(iEditor)
	, mGUI(nullptr)
	, mPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased)
    , mActor(nullptr)
    , mComponent(nullptr)
    , mMaterial(nullptr)
{
	int a = 0;
}

void
FOdysseyViewportDrawingEditorExtension::Initialize()
{
	//Handle Object Property Changed Callback to refresh when actors's visibility changes for example
    FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this,&FOdysseyViewportDrawingEditorExtension::OnObjectPropertyChanged);
}

void
FOdysseyViewportDrawingEditorExtension::Finalize()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

AActor*
FOdysseyViewportDrawingEditorExtension::Actor() const
{
    return mActor;
}

UMeshComponent*
FOdysseyViewportDrawingEditorExtension::Component() const
{
    return mComponent;
}

UMaterialInterface*
FOdysseyViewportDrawingEditorExtension::Material() const
{
    return mMaterial;
}

UTexture2D*
FOdysseyViewportDrawingEditorExtension::Texture() const
{
	TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyTextureEditorSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyTextureEditorSource> textureSource = StaticCastSharedPtr<FOdysseyTextureEditorSource>(source);
	return textureSource->GetTexture();
}

const TArray<UMeshComponent*>&
FOdysseyViewportDrawingEditorExtension::SelectableComponents() const
{
    return mSelectableComponents;
}

void
FOdysseyViewportDrawingEditorExtension::SelectableMaterials( TArray<UMaterialInterface*>& ioSelectableMaterials ) const
{
    if ( mComponent )
    {
        mComponent->GetUsedMaterials( ioSelectableMaterials );
    }
}

const TArray<FPaintableTexture>&
FOdysseyViewportDrawingEditorExtension::SelectableTextures() const
{
    return mSelectableTextures;
}

const TMap<UMeshComponent*, TSharedPtr<IMeshPaintGeometryAdapter>>&
FOdysseyViewportDrawingEditorExtension::ComponentToAdapterMap() const
{
	return mComponentToAdapterMap;
}

EOdysseyViewportDrawingPaintingAdapterMethod FOdysseyViewportDrawingEditorExtension::PaintingAdapterMethod() const
{
	return mPaintingAdapterMethod;
}


int32 FOdysseyViewportDrawingEditorExtension::GetUVIndexUsedByCurrentTexture()
{
	if (mMaterial != NULL)
	{
        for (UMaterialExpression* expression : mMaterial->GetMaterial()->GetExpressions())
        {
			UMaterialExpressionTextureBase* TextureBase = Cast<UMaterialExpressionTextureBase>(expression);
			if (TextureBase != NULL &&
				TextureBase->Texture != NULL &&
				TextureBase->Texture == Texture() )
			{
				UMaterialExpressionTextureSample* TextureSample = Cast<UMaterialExpressionTextureSample>(expression);
				if (TextureSample != NULL)
				{
					UMaterialExpressionTextureCoordinate* TextureCoords = Cast<UMaterialExpressionTextureCoordinate>(TextureSample->Coordinates.Expression);
					if (TextureCoords != NULL)
					{
						return TextureCoords->CoordinateIndex;
					}
					else
					{
						return TextureSample->ConstCoordinate;
					}
				}
			}
		}
	}
	return 0;
}

float FOdysseyViewportDrawingEditorExtension::GetMeshComponentMaxSize() const
{
    if (mComponent)
    {
        FVector extent = mComponent->GetLocalBounds().BoxExtent;
		FVector scale = mActor->GetActorScale();
        return FMath::Max3(extent.X, extent.Y, extent.Z) * FMath::Max3(scale.X, scale.Y, scale.Z);
    }
    return 1;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyViewportDrawingEditorExtension::SetActor(AActor* iActor)
{
	if (mActor == iActor)
		return;

	mTargetToPaintWillChangeDelegate.Broadcast();

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
FOdysseyViewportDrawingEditorExtension::SetComponent(UMeshComponent* iComponent)
{
	if (mComponent == iComponent)
		return;

    mTargetToPaintWillChangeDelegate.Broadcast();

	// Save Component Paint Settings
	if (mComponent)
	{
		FInstanceTexturePaintSettings& texturePaintSettings = mComponentToTexturePaintSettingsMap.FindOrAdd(mComponent);
		texturePaintSettings.mSelectedTexture = Texture();
	}

	// Change the selected component
	mComponent = iComponent;
    SelectDefaultMaterial();
}

void
FOdysseyViewportDrawingEditorExtension::SetMaterial(UMaterialInterface* iMaterial)
{
	if( iMaterial == mMaterial )
		return;

    mTargetToPaintWillChangeDelegate.Broadcast();

    mMaterial = iMaterial;
    ClearSelectableTextures(); // Also clears selected Texture
    UpdateSelectableTextures();
    
    if (mMaterial)
        SelectDefaultTexture();
}

void
FOdysseyViewportDrawingEditorExtension::SetTexture(UTexture2D* iTexture)
{
	mTargetToPaintWillChangeDelegate.Broadcast();
	/* UTexture2D* texture = Texture();
	if ( texture )
		RemoveEditedObject(texture); */

	mEditor->SetSource(nullptr);
	if (iTexture)
	{
		TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(iTexture);
		mEditor->SetSource(source);
	}

	/* texture = Texture();
	if ( texture )
		AddEditedObject(texture); */

	mTargetToPaintChangedDelegate.Broadcast();
}

void FOdysseyViewportDrawingEditorExtension::SetPaintingAdapterMethod(EOdysseyViewportDrawingPaintingAdapterMethod iNewMethod)
{
	mPaintingAdapterMethod = iNewMethod;
	mAdapterChangedDelegate.Broadcast();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyViewportDrawingEditorGUI*
FOdysseyViewportDrawingEditorExtension::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShared<FOdysseyViewportDrawingEditorGUI>(this);
	return mGUI.Get();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Private Methods

void
FOdysseyViewportDrawingEditorExtension::OnObjectPropertyChanged(UObject* iObject, struct FPropertyChangedEvent& iPropertyChangedEvent)
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


FOdysseyViewportDrawingEditorExtension::FOdysseyPaintingTargetToPaintWillChange& FOdysseyViewportDrawingEditorExtension::TargetToPaintWillChangeDelegate()
{
    return mTargetToPaintWillChangeDelegate;
}

FOdysseyViewportDrawingEditorExtension::FOdysseyPaintingTargetToPaintChanged& FOdysseyViewportDrawingEditorExtension::TargetToPaintChangedDelegate()
{
	return mTargetToPaintChangedDelegate;
}

FOdysseyViewportDrawingEditorExtension::FOdysseyPaintingAdapterChanged& FOdysseyViewportDrawingEditorExtension::AdapterChangedDelegate()
{
	return mAdapterChangedDelegate;
}

void
FOdysseyViewportDrawingEditorExtension::ClearSelectableComponents()
{
	SetComponent(nullptr);
	mSelectableComponents.Empty();
	for (auto meshAdapterPair : mComponentToAdapterMap)
	{
		meshAdapterPair.Value->OnRemoved();
	}
	mComponentToAdapterMap.Empty();
	FMeshPaintAdapterFactory::CleanupGlobals();
}

void
FOdysseyViewportDrawingEditorExtension::UpdateSelectableComponents()
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
FOdysseyViewportDrawingEditorExtension::SelectDefaultComponent()
{
	//TODO: make it smarter, like SelectDefaultTexture does
	// for now we only select the first component of the selected actor

	if (!mActor)
		return;

	if (mSelectableComponents.Num() <= 0)
		return;

	SetComponent(mSelectableComponents[0]);
}

void
FOdysseyViewportDrawingEditorExtension::ClearSelectableTextures()
{
	SetTexture(nullptr);
	mSelectableTextures.Empty();
}

void
FOdysseyViewportDrawingEditorExtension::UpdateSelectableTextures()
{
	//make sure the array in empty
	ClearSelectableTextures();

	if (!mComponent)
		return;

	TSharedPtr<IMeshPaintGeometryAdapter> adapter = mComponentToAdapterMap.FindChecked(mComponent);
	FOdysseyViewportDrawingEditorUtils::RetrieveTexturesForComponent(mComponent, mSelectableTextures);
}

void
FOdysseyViewportDrawingEditorExtension::SelectDefaultMaterial()
{
    if (!mComponent)
        SetMaterial(nullptr);
    else
    {
        TArray<UMaterialInterface*> materialsArray;
        SelectableMaterials( materialsArray );
        if ( materialsArray.Num() > 0)
            SetMaterial( materialsArray[0] );
    }
}

void
FOdysseyViewportDrawingEditorExtension::SelectDefaultTexture()
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
			SetTexture(texturePaintSettings.mSelectedTexture);
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

		SetTexture(texture);
		break;
	}
}


#undef LOCTEXT_NAMESPACE