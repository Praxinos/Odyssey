// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureWrapper.h"

FOdysseyTextureWrapper::~FOdysseyTextureWrapper()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
    UPackage::PreSavePackageEvent.Remove(mOnPackagePreSaveHandle);
    UPackage::PackageSavedEvent.Remove(mOnPackageSavedHandle);
}

FOdysseyTextureWrapper::FOdysseyTextureWrapper()
{
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyTextureWrapper::OnPreGlobalObjectPropertyChanged);
    mOnPackagePreSaveHandle = UPackage::PreSavePackageEvent.AddRaw(this, &FOdysseyTextureWrapper::OnPackagePreSave);
	mOnPackageSavedHandle = UPackage::PackageSavedEvent.AddRaw(this, &FOdysseyTextureWrapper::OnPackageSaved);
}

void
FOdysseyTextureWrapper::PreTextureChange()
{
    mOnPreTextureChange.Broadcast();

    UpdateTextureFromSurface();

    mOnLayerStackFinalize.Broadcast();
    FinalizeLayerStack();

    mOnSurfaceFinalize.Broadcast();
    FinalizeSurface();

    RestoreTextureProperties();
}

void
FOdysseyTextureWrapper::PostTextureChange()
{
    SetTextureProperties();

    InitializeSurface();
    mOnSurfaceInitialize.Broadcast();

    InitializeLayerStack();
    mOnLayerStackInitialize.Broadcast();

    mOnPostTextureChange.Broadcast();
}

typename FOdysseyTextureWrapper::FOnPrePropertyChanged&
FOdysseyTextureWrapper::OnPrePropertyChangedDelegate()
{
    return mOnPrePropertyChanged;
}

typename FOdysseyTextureWrapper::FOnPreSave&
FOdysseyTextureWrapper::OnPreSaveDelegate()
{
    return mOnPreSave;
}

typename FOdysseyTextureWrapper::FOnPostSave&
FOdysseyTextureWrapper::OnPostSaveDelegate()
{
    return mOnPostSave;
}

typename FOdysseyTextureWrapper::FOnPreTextureChange&
FOdysseyTextureWrapper::OnPreTextureChangeDelegate()
{
    return mOnPreTextureChange;
}

typename FOdysseyTextureWrapper::FOnPostTextureChange&
FOdysseyTextureWrapper::OnPostTextureChangeDelegate()
{
    return mOnPostTextureChange;
}

void
FOdysseyTextureWrapper::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    UTexture* texture = Texture();
    if (!texture)
        return;

    if (texture != Cast<UTexture>(iObject))
        return;

    UpdateTextureFromSurface();
    mOnPrePropertyChanged.Broadcast(iEditPropertyChain);
}

void
FOdysseyTextureWrapper::OnPackagePreSave(UPackage* iPackage)
{
    UTexture* texture = Texture();
    if (!texture)
        return;

    UPackage* package = CastChecked<UPackage>(texture->GetOuter());
    if (package != iPackage)
        return;

    mOnPreSave.Broadcast();
	UpdateTextureFromSurface();
	RestoreTextureProperties();
}

void
FOdysseyTextureWrapper::OnPackageSaved(const FString& iPackageFilename, UObject* iOuter)
{
    UTexture* texture = Texture();
    if (!texture)
        return;

    if (texture->GetOuter() != iOuter)
        return;

    mOnPostSave.Broadcast();
    SetTextureProperties();
}
