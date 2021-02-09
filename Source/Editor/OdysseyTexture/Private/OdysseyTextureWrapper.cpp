// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureWrapper.h"

FOdysseyTextureWrapper::~FOdysseyTextureWrapper()
{
    Texture(nullptr);

    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
    UPackage::PreSavePackageEvent.Remove(mOnPackagePreSaveHandle);
    UPackage::PackageSavedEvent.Remove(mOnPackageSavedHandle);
}

FOdysseyTextureWrapper::FOdysseyTextureWrapper(UTexture2D* iTexture) :
    mTexture(nullptr),
    mSurface(nullptr)
{
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyTextureWrapper::OnPreGlobalObjectPropertyChanged);
    mOnPackagePreSaveHandle = UPackage::PreSavePackageEvent.AddRaw(this, &FOdysseyTextureWrapper::OnPackagePreSave);
	mOnPackageSavedHandle = UPackage::PackageSavedEvent.AddRaw(this, &FOdysseyTextureWrapper::OnPackageSaved);

    Texture(iTexture);
}

void
FOdysseyTextureWrapper::Texture(UTexture2D* iTexture)
{
    if (iTexture == mTexture)
        return;

    mOnPreTextureChange.Broadcast(iTexture);

    UTexture2D* oldTexture = mTexture;

    UpdateTextureFromSurface();
	RestoreTextureProperties();

    mTexture = nullptr;
    if (mSurface)
    {
		mSurface->Block()->GetBlock()->SetOnInvalid(::ul3::FOnInvalid());
        delete mSurface;
        mSurface = nullptr;
	}

    if (iTexture)
    {
        mTexture = iTexture;
        
        //Prepare the texture to be edited and store its properties in a backup structure
        SetTextureProperties();

        // Setup Surface
        mSurface = new FOdysseySurfaceEditable(mTexture);
        mSurface->Invalidate();
    }

    mOnPostTextureChange.Broadcast(oldTexture);
}

UTexture2D*
FOdysseyTextureWrapper::Texture() const
{
    return mTexture;
}

FOdysseySurfaceEditable*
FOdysseyTextureWrapper::Surface() const
{
    return mSurface;
}

FOdysseyLayerStack*
FOdysseyTextureWrapper::LayerStack() const
{
    if (!mTexture)
        return nullptr;

    UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(mTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
    if( !userData )
    {
        userData = CreateTextureUserData(mTexture);
    }

    return userData->GetLayerStack();
}

FOdysseyTextureWrapper::FOnPrePropertyChanged&
FOdysseyTextureWrapper::OnPrePropertyChangedDelegate()
{
    return mOnPrePropertyChanged;
}

FOdysseyTextureWrapper::FOnPreSave&
FOdysseyTextureWrapper::OnPreSaveDelegate()
{
    return mOnPreSave;
}

FOdysseyTextureWrapper::FOnPostSave&
FOdysseyTextureWrapper::OnPostSaveDelegate()
{
    return mOnPostSave;
}

FOdysseyTextureWrapper::FOnPreTextureChange&
FOdysseyTextureWrapper::OnPreTextureChangeDelegate()
{
    return mOnPreTextureChange;
}

FOdysseyTextureWrapper::FOnPostTextureChange&
FOdysseyTextureWrapper::OnPostTextureChangeDelegate()
{
    return mOnPostTextureChange;
}

UOdysseyTextureAssetUserData*
FOdysseyTextureWrapper::CreateTextureUserData(UTexture2D* iTexture) const
{
    //Init user data
    ::ul3::tFormat format = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
    UOdysseyTextureAssetUserData* userData = NewObject< UOdysseyTextureAssetUserData >(iTexture, NAME_None, RF_Public);
    userData->GetLayerStack()->Init(iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY(), format);
    iTexture->AddAssetUserData( userData );

    //Create image layer
    FOdysseyBlock* textureData = NewOdysseyBlockFromUTextureData( iTexture, userData->GetLayerStack()->Format() );
    FName layerName = userData->GetLayerStack()->GetLayerRoot()->GetNextLayerName();
    TSharedPtr<FOdysseyImageLayer> imageLayer = MakeShareable(new FOdysseyImageLayer(layerName, textureData));

    //Add Layer
    userData->GetLayerStack()->AddLayer(imageLayer);

    // Notify for changes
    iTexture->PostEditChange();
    return userData;
}

void
FOdysseyTextureWrapper::UpdateTextureFromSurface()
{
    if(!mTexture)
        return;

    CopyBlockDataIntoUTexture( mSurface->Block(), mTexture );
    mTexture->UpdateResource();
}

void
FOdysseyTextureWrapper::SetTextureProperties()
{
    if(!mTexture)
        return;

    FTextureFormatSettings textureFormatSettings;
	mTexture->GetLayerFormatSettings(0, textureFormatSettings);

	// Create new Texture Properties Backup
	mPropertyCompressionNone = textureFormatSettings.CompressionNone;

	// Overwrite Texture properties
    textureFormatSettings.CompressionNone = 1;
	mTexture->SetLayerFormatSettings(0, textureFormatSettings);

	mTexture->UpdateResource();
    mTexture->TemporarilyDisableStreaming(); //needed to be able to draw on previously streamed textures, avoids using NoMipMaps
}

void
FOdysseyTextureWrapper::RestoreTextureProperties()
{
    if(!mTexture)
        return;

    FTextureFormatSettings textureFormatSettings;
    mTexture->GetLayerFormatSettings(0,textureFormatSettings);
    textureFormatSettings.CompressionNone = mPropertyCompressionNone;
    mTexture->SetLayerFormatSettings(0, textureFormatSettings);

    mTexture->UpdateResource();
}

void
FOdysseyTextureWrapper::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    if (!mTexture)
        return;

    if (mTexture != Cast<UTexture2D>(iObject))
        return;

    CopyBlockDataIntoUTexture( mSurface->Block(), mTexture );

    mOnPrePropertyChanged.Broadcast(iEditPropertyChain);
}

void
FOdysseyTextureWrapper::OnPackagePreSave(UPackage* iPackage)
{
    if (!mTexture)
        return;

    UPackage* package = CastChecked<UPackage>(mTexture->GetOuter());
    if (package != iPackage)
        return;

    mOnPreSave.Broadcast();
	UpdateTextureFromSurface();
	RestoreTextureProperties();
}

void
FOdysseyTextureWrapper::OnPackageSaved(const FString& iPackageFilename, UObject* iOuter)
{
    if (!mTexture)
        return;

    if (mTexture->GetOuter() != iOuter)
        return;

    mOnPostSave.Broadcast();
    SetTextureProperties();
}