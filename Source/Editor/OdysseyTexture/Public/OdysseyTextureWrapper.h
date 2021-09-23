// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include <ULIS3>

class UTexture;
class FOdysseyLayerStack;
class IOdysseySurfaceEditable;

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

    DECLARE_MULTICAST_DELEGATE(FOnPreTextureChange);
    DECLARE_MULTICAST_DELEGATE(FOnPostTextureChange);
    DECLARE_MULTICAST_DELEGATE(FOnLayerStackInitialize);
    DECLARE_MULTICAST_DELEGATE(FOnLayerStackFinalize);
    DECLARE_MULTICAST_DELEGATE(FOnSurfaceInitialize);
    DECLARE_MULTICAST_DELEGATE(FOnSurfaceFinalize);

public:
    /** The destructor */
	virtual ~FOdysseyTextureWrapper();

    /** The constructor */
	FOdysseyTextureWrapper();

public:
    //Getters
    virtual UTexture* Texture() const = 0;
    virtual IOdysseySurfaceEditable* Surface() const = 0;
    virtual FOdysseyLayerStack* LayerStack() const = 0;

public:
    //Public FOdysseyTextureWrapper interface
    virtual void Finalize() = 0;

public:
    //Delegates
    FOnPrePropertyChanged& OnPrePropertyChangedDelegate();
    FOnPreSave& OnPreSaveDelegate();
    FOnPostSave& OnPostSaveDelegate();
    FOnPreTextureChange& OnPreTextureChangeDelegate();
    FOnPostTextureChange& OnPostTextureChangeDelegate();

protected:
    //Private Wrapper Interface
    //Manage Texture Synchronization with Surface
    virtual void UpdateTextureFromSurface() = 0;

    //Prepares Texture for use in an OdysseyEditor
	virtual void SetTextureProperties() = 0;

    //Restores Original Texture properties
	virtual void RestoreTextureProperties() = 0;

    //Surface Initialization
    virtual void InitializeSurface() = 0;

    //Surface Finalization
    virtual void FinalizeSurface() = 0;

    //LayerStack Initialization
    virtual void InitializeLayerStack() = 0;

    //LayerStack Finalization
    virtual void FinalizeLayerStack() = 0;

protected:
    void PreTextureChange();
    void PostTextureChange();

private:
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage);
    void OnPackageSaved(const FString& iPackageFilename, UObject* iOuter);

private:
	// UTexture* mTexture;
    // IOdysseySurfaceEditable* mSurface;
    
	FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPackagePreSaveHandle;
    FDelegateHandle mOnPackageSavedHandle;

    //Delegates
    FOnPrePropertyChanged mOnPrePropertyChanged;
    FOnPreSave mOnPreSave;
    FOnPostSave mOnPostSave;

    FOnPreTextureChange mOnPreTextureChange;
    FOnPostTextureChange mOnPostTextureChange;
    FOnLayerStackInitialize mOnLayerStackInitialize;
    FOnLayerStackFinalize mOnLayerStackFinalize;
    FOnSurfaceInitialize mOnSurfaceInitialize;
    FOnSurfaceFinalize mOnSurfaceFinalize;
};
