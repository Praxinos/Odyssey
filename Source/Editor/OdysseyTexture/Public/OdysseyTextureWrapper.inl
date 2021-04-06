// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

template<class T>
FOdysseyTextureWrapper<T>::~FOdysseyTextureWrapper()
{
    FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(mOnPrePropertyChangedDelegateHandle);
    UPackage::PreSavePackageEvent.Remove(mOnPackagePreSaveHandle);
    UPackage::PackageSavedEvent.Remove(mOnPackageSavedHandle);
}

template<class T>
FOdysseyTextureWrapper<T>::FOdysseyTextureWrapper() :
    mTexture(nullptr),
    mSurface(nullptr)
{
    mOnPrePropertyChangedDelegateHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FOdysseyTextureWrapper::OnPreGlobalObjectPropertyChanged);
    mOnPackagePreSaveHandle = UPackage::PreSavePackageEvent.AddRaw(this, &FOdysseyTextureWrapper::OnPackagePreSave);
	mOnPackageSavedHandle = UPackage::PackageSavedEvent.AddRaw(this, &FOdysseyTextureWrapper::OnPackageSaved);
}

template<class T>
void
FOdysseyTextureWrapper<T>::Texture(T* iTexture)
{
    if (iTexture == mTexture)
        return;

    mOnPreTextureChange.Broadcast(iTexture);

    T* oldTexture = mTexture;

    UpdateTextureFromSurface();
	RestoreTextureProperties();

    mTexture = nullptr;
    DestroySurface();
    mSurface = nullptr;

    if (iTexture)
    {
        mTexture = iTexture;
        
        //Prepare the texture to be edited and store its properties in a backup structure
        SetTextureProperties();

        // Setup Surface
        mSurface = CreateSurface();
    }

    mOnPostTextureChange.Broadcast(oldTexture);
}

template<class T>
T*
FOdysseyTextureWrapper<T>::Texture() const
{
    return mTexture;
}

template<class T>
IOdysseySurfaceEditable*
FOdysseyTextureWrapper<T>::Surface() const
{
    return mSurface;
}

template<class T>
typename FOdysseyTextureWrapper<T>::FOnPrePropertyChanged&
FOdysseyTextureWrapper<T>::OnPrePropertyChangedDelegate()
{
    return mOnPrePropertyChanged;
}

template<class T>
typename FOdysseyTextureWrapper<T>::FOnPreSave&
FOdysseyTextureWrapper<T>::OnPreSaveDelegate()
{
    return mOnPreSave;
}

template<class T>
typename FOdysseyTextureWrapper<T>::FOnPostSave&
FOdysseyTextureWrapper<T>::OnPostSaveDelegate()
{
    return mOnPostSave;
}

template<class T>
typename FOdysseyTextureWrapper<T>::FOnPreTextureChange&
FOdysseyTextureWrapper<T>::OnPreTextureChangeDelegate()
{
    return mOnPreTextureChange;
}

template<class T>
typename FOdysseyTextureWrapper<T>::FOnPostTextureChange&
FOdysseyTextureWrapper<T>::OnPostTextureChangeDelegate()
{
    return mOnPostTextureChange;
}

template<class T>
void
FOdysseyTextureWrapper<T>::OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain)
{
    if (!mTexture)
        return;

    if (mTexture != Cast<T>(iObject))
        return;

    UpdateTextureFromSurface();
    mOnPrePropertyChanged.Broadcast(iEditPropertyChain);
}

template<class T>
void
FOdysseyTextureWrapper<T>::OnPackagePreSave(UPackage* iPackage)
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

template<class T>
void
FOdysseyTextureWrapper<T>::OnPackageSaved(const FString& iPackageFilename, UObject* iOuter)
{
    if (!mTexture)
        return;

    if (mTexture->GetOuter() != iOuter)
        return;

    mOnPostSave.Broadcast();
    SetTextureProperties();
}
