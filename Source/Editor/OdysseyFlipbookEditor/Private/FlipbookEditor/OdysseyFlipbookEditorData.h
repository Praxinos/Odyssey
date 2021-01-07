// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorData.h"
#include "OdysseySurfaceReadOnly.h"

class UPaperFlipbook;
class UTexture2D;
class UOdysseyBrush;
class UOdysseyTextureAssetUserData;
class FOdysseyFlipbookEditorToolkit;
class FOdysseyFlipbookWrapper;

struct FTexturePropertiesBackup
{
    uint8 mTextureCompressionNone;
};

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class FOdysseyFlipbookEditorData :
	public FOdysseyPainterEditorData
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorData();
    FOdysseyFlipbookEditorData(TSharedPtr<FOdysseyFlipbookWrapper>& iFlipbookWrapper, TSharedPtr<FOdysseyFlipbookEditorToolkit> iToolkit);

public:
    //Overrides
	virtual void Init() override;
	virtual FOdysseySurfaceEditable*    DisplaySurface() override;

    //Getters

    //Currently Edited Flipbook
	TSharedPtr<FOdysseyFlipbookWrapper>& FlipbookWrapper();

    //Currently Edited Texture
    UTexture2D*							Texture();

    //Currently Edited LayerStack
    FOdysseyLayerStack*					LayerStack() const;
    
	FOdysseySurfaceReadOnly*			PreviewSurface();

    TWeakPtr<FOdysseyFlipbookEditorToolkit>& Toolkit();

    //Data Sanity Methods 

    // Sets the currently edited texture
    void                        Texture(UTexture2D* iTexture);
    
private:
    UOdysseyTextureAssetUserData* FindOrCreateTextureUserData(UTexture2D* iTexture);

public:
    // Syncs the texture data with the surface block data
    // This ensures the texture contains the right data (before saving for example)
	void SyncTextureWithSurfaceBlock();

	void ApplyPropertiesBackup();
    
    void PrepareTextureProperties();

    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage);
    void OnPackageSaved(const FString& iPackageFilename, UObject* iOuter);

private:
    TSharedPtr<FOdysseyFlipbookWrapper>             mFlipbookWrapper;
    UTexture2D*                 mTexture;

    FOdysseyLayerStack*         mLayerStack;  // Copied from mOdysseyTexture AssetUserData
    
    //Recreated everytime the current Keyframe changes
	FOdysseySurfaceEditable*            mDisplaySurface;

	//Surface for previewing in viewport
	FOdysseySurfaceReadOnly*            mPreviewSurface;

    //Current Texture Properties backup
    FTexturePropertiesBackup    mPropertiesBackup;

    TWeakPtr<FOdysseyFlipbookEditorToolkit> mToolkit;
	FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPackagePreSaveHandle;
    FDelegateHandle mOnPackageSavedHandle;
};

