// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorData.h"

class UTexture;
class UOdysseyBrush;

struct FTexturePropertiesBackup
{
    uint8 mTextureCompressionNone;
};

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyTextureEditorData :
	public FOdysseyPainterEditorData
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorData();
    FOdysseyTextureEditorData(UTexture2D* iTexture);

public:
	virtual void Init() override;

    // Paint engine driving methods
	UTexture2D*							Texture();
    FOdysseyLayerStack*					LayerStack();
	virtual FOdysseySurfaceEditable*    DisplaySurface() override;

public:
	void SyncTextureAndInvalidate();
	void PrepareTextureProperties();
	void ApplyPropertiesBackup();

    void OnPreGlobalObjectPropertyChanged(UObject* iObject, const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage);
    void OnPackageSaved(const FString& iPackageFilename, UObject* iOuter);

private:
    UTexture2D*                 mTexture;
    FOdysseyLayerStack*         mLayerStack;        // Copied from mOdysseyTexture AssetUserData
	FOdysseySurfaceEditable*    mDisplaySurface;

    FTexturePropertiesBackup    mPropertiesBackup;
	FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPackagePreSaveHandle;
    FDelegateHandle mOnPackageSavedHandle;
};

