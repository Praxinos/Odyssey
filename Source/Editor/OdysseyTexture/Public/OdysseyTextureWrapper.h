// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include <ULIS3>

class UTexture2D;
class FOdysseySurfaceEditable;
class FOdysseyLayerStack;
class UOdysseyTextureAssetUserData;

/** 
 * High level wrapper class to modify or read a UTexture2D
*/
class ODYSSEYTEXTURE_API FOdysseyTextureWrapper : public TSharedFromThis<FOdysseyTextureWrapper>
{
public:
    //Delegates
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPrePropertyChanged, const FEditPropertyChain&);
    DECLARE_MULTICAST_DELEGATE(FOnPreSave);
    DECLARE_MULTICAST_DELEGATE(FOnPostSave);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPreTextureChange, UTexture2D*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostTextureChange, UTexture2D*);

public:
    /** The destructor */
	~FOdysseyTextureWrapper();

    /** The constructor */
	FOdysseyTextureWrapper(UTexture2D* iTexture);
    
public:
    void Texture(UTexture2D*);
    UTexture2D* Texture();
    FOdysseySurfaceEditable* Surface();
    FOdysseyLayerStack* LayerStack();

    FOnPrePropertyChanged& OnPrePropertyChangedDelegate();
    FOnPreSave& OnPreSaveDelegate();
    FOnPostSave& OnPostSaveDelegate();
    FOnPreTextureChange& OnPreTextureChangeDelegate();
    FOnPostTextureChange& OnPostTextureChangeDelegate();

private:
    //Creates Texture UserData holding the layerstack for example and returns it
    UOdysseyTextureAssetUserData* CreateTextureUserData(UTexture2D* iTexture);

    //Manage Texture Synchronization with Surface
    void UpdateTextureFromSurface();

    //Prepares Texture for use in an OdysseyEditor
	void SetTextureProperties();

    //Restores Original Texture properties
	void RestoreTextureProperties();

private:
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage);
    void OnPackageSaved(const FString& iPackageFilename, UObject* iOuter);

private:
	UTexture2D* mTexture;
    FOdysseySurfaceEditable* mSurface;
    
	FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPackagePreSaveHandle;
    FDelegateHandle mOnPackageSavedHandle;

    //Delegates
    FOnPrePropertyChanged mOnPrePropertyChanged;
    FOnPreSave mOnPreSave;
    FOnPostSave mOnPostSave;

    FOnPreTextureChange mOnPreTextureChange;
    FOnPostTextureChange mOnPostTextureChange;

    //tmp
    int mPropertyCompressionNone;
};