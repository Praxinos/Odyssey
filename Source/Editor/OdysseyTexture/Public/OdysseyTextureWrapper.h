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
template<class T>
class FOdysseyTextureWrapper : public TSharedFromThis<FOdysseyTextureWrapper<T>>
{
public:
    //Delegates
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPrePropertyChanged, const FEditPropertyChain&);
    DECLARE_MULTICAST_DELEGATE(FOnPreSave);
    DECLARE_MULTICAST_DELEGATE(FOnPostSave);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPreTextureChange, T*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostTextureChange, T*);

public:
    /** The destructor */
	virtual ~FOdysseyTextureWrapper();

    /** The constructor */
	FOdysseyTextureWrapper();
    
public:
    //Generic Setters/Getters
    //Set Texture
    void Texture(T*);

    //Get Texture
    T* Texture() const;

    //Get Surface
    IOdysseySurfaceEditable* Surface() const;

    //Delegates
    FOnPrePropertyChanged& OnPrePropertyChangedDelegate();
    FOnPreSave& OnPreSaveDelegate();
    FOnPostSave& OnPostSaveDelegate();
    FOnPreTextureChange& OnPreTextureChangeDelegate();
    FOnPostTextureChange& OnPostTextureChangeDelegate();

public:
    //Public Wrapper Interface
    //Get LayerStack
    virtual FOdysseyLayerStack* LayerStack() const = 0;

protected:
    //Private Wrapper Interface
    //Manage Texture Synchronization with Surface
    virtual void UpdateTextureFromSurface() = 0;

    //Prepares Texture for use in an OdysseyEditor
	virtual void SetTextureProperties() = 0;

    //Restores Original Texture properties
	virtual void RestoreTextureProperties() = 0;

    //Destroys properly the surface
    virtual void DestroySurface() = 0;

    //Creates the appropriate surface
    virtual IOdysseySurfaceEditable* CreateSurface() = 0;

private:
    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage);
    void OnPackageSaved(const FString& iPackageFilename, UObject* iOuter);

private:
	T* mTexture;
    IOdysseySurfaceEditable* mSurface;
    
	FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPackagePreSaveHandle;
    FDelegateHandle mOnPackageSavedHandle;

    //Delegates
    FOnPrePropertyChanged mOnPrePropertyChanged;
    FOnPreSave mOnPreSave;
    FOnPostSave mOnPostSave;

    FOnPreTextureChange mOnPreTextureChange;
    FOnPostTextureChange mOnPostTextureChange;
};

#include "OdysseyTextureWrapper.inl"
