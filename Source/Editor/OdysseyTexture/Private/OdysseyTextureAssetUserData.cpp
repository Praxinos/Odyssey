// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureAssetUserData.h"
#include "UObject/ObjectSaveContext.h"

#include "EngineUtils.h"

UOdysseyTextureAssetUserData::UOdysseyTextureAssetUserData()
{
    mLayerStack = new FOdysseyLayerStack();
}

//-------------------------
//UObject Interface--------
void UOdysseyTextureAssetUserData::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    Ar << mLayerStack;
}
//-------------------------
//End UObject Interface----

FOdysseyLayerStack* UOdysseyTextureAssetUserData::GetOldLayerStack()
{
    return mLayerStack;
}

void UOdysseyTextureAssetUserData::SetOldLayerStack(FOdysseyLayerStack* iLayerStack)
{
    if( !iLayerStack )
        return;

    if( mLayerStack )
        delete mLayerStack;

    mLayerStack = iLayerStack;
}

UOdysseyTextureLayerStack*
UOdysseyTextureAssetUserData::GetLayerStack()
{
    if (!LayerStack)
        LayerStack = UOdysseyTextureLayerStack::CreateFromTexture(GetTexture(), this);
    return LayerStack;
}

UTexture2D*
UOdysseyTextureAssetUserData::GetTexture()
{
    return Cast<UTexture2D>(GetOuter());
}

/*

TSharedPtr<FOdysseySurfaceTexture2DEditable>
UOdysseyTextureAssetUserData::GetSurface() const
{
    return mTextureFastUpdateSurface;
}

void
UOdysseyTextureAssetUserData::StartEdit()
{
    mIsEditing = true;
    mTextureFastUpdateSurface = MakeShared<FOdysseySurfaceTexture2DEditable>(Texture);
    ActivateTextureFastUpdate();

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddUObject(this, &UOdysseyTextureAssetUserData::OnPreGlobalObjectPropertyChanged);
    UPackage::PreSavePackageWithContextEvent.AddUObject(this, &UOdysseyTextureAssetUserData::OnPackagePreSave);
	UPackage::PackageSavedWithContextEvent.AddUObject(this, &UOdysseyTextureAssetUserData::OnPackageSaved);
}

void
UOdysseyTextureAssetUserData::StopEdit()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.RemoveAll(this);
    UPackage::PreSavePackageWithContextEvent.RemoveAll(this);
    UPackage::PackageSavedWithContextEvent.RemoveAll(this);
    InactivateTextureFastUpdate();
    UpdateTextureSource();
    mIsEditing = false;
    mTextureFastUpdateSurface = nullptr;
}

void
UOdysseyTextureAssetUserData::ActivateTextureFastUpdate()
{
    FTextureFormatSettings textureFormatSettings;
    Texture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mTextureCompressionNone = textureFormatSettings.CompressionNone;

	// Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
    Texture->SetLayerFormatSettings(0, textureFormatSettings);

    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({Texture});
    Texture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
    //FastUpdateTexture({ ::ULIS::FRectI::FromXYWH(0, 0, Texture->Source.GetSizeX(), Texture->Source.GetSizeY()) });
}

void
UOdysseyTextureAssetUserData::InactivateTextureFastUpdate()
{
    UOdysseyTextureLayerStack::OnRenderImageChanged().RemoveAll(this);

    FTextureFormatSettings textureFormatSettings;
    Texture->GetLayerFormatSettings(0,textureFormatSettings);
    textureFormatSettings.CompressionNone = mTextureCompressionNone;
    Texture->SetLayerFormatSettings(0, textureFormatSettings);
    UpdateTextureSource();
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
}

void
UOdysseyTextureAssetUserData::FastUpdateTexture(const TArray<::ULIS::FRectI>& iRects)
{
    FTextureCompilingManager::Get().FinishCompilation({Texture});

    for (int i = 0; i < iRects.Num(); i++ )
        LayerStack->RenderImage(mTextureFastUpdateSurface->Block(), iRects[i], iRects[i].Position(), TArray<::ULIS::FEvent>());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mTextureFastUpdateSurface->Block()->Format());
    ctx.Finish();

    mTextureFastUpdateSurface->Invalidate(iRects);
}

void
UOdysseyTextureAssetUserData::UpdateTextureSource()
{
    CopyBlockDataIntoUTexture( mTextureFastUpdateSurface->Block(), Texture );
}

void
UOdysseyTextureAssetUserData::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    if (Texture != Cast<UTexture2D>(iObject))
        return;

    UpdateTextureSource();
    // Texture->UpdateResource();
    //FTextureCompilingManager::Get().FinishCompilation({Texture});
}

void
UOdysseyTextureAssetUserData::OnPackagePreSave(UPackage* iPackage, FObjectPreSaveContext ObjectSaveContext)
{
    UPackage* package = CastChecked<UPackage>(Texture->GetOuter());
    if (package != iPackage)
        return;

	InactivateTextureFastUpdate();
}

void
UOdysseyTextureAssetUserData::OnPackageSaved(const FString& iPackageFilename, UPackage* iPackage, FObjectPostSaveContext ObjectSaveContext)
{
    UPackage* package = CastChecked<UPackage>(Texture->GetOuter());
    if (package != iPackage)
        return;

	ActivateTextureFastUpdate();
}
*/