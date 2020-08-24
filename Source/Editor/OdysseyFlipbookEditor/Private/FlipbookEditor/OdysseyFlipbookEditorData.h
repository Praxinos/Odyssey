// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorData.h"
#include "OdysseySurfaceReadOnly.h"

class UPaperFlipbook;
class UTexture2D;
class UOdysseyBrush;
class UOdysseyTextureAssetUserData;

struct FTexturePropertiesBackup
{
    TextureMipGenSettings       mTextureMipGenBackup;
    TextureCompressionSettings  mTextureCompressionBackup;
    TextureGroup                mTextureGroupBackup;
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
    FOdysseyFlipbookEditorData(UPaperFlipbook* iFlipbook);

public:
    //Overrides
	virtual void Init() override;
	virtual FOdysseySurfaceEditable*    DisplaySurface() override;

    //Getters

    //Currently Edited Flipbook
	UPaperFlipbook*						Flipbook();

    //Currently Edited Texture
    UTexture2D*							Texture();

    //Currently Edited LayerStack
    FOdysseyLayerStack*					LayerStack() const;
    
	FOdysseySurfaceReadOnly*			PreviewSurface();

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

private:
    UPaperFlipbook*             mFlipbook;
    UTexture2D*                 mTexture;

    FOdysseyLayerStack*         mLayerStack;  // Copied from mOdysseyTexture AssetUserData
    
    //Recreated everytime the current Keyframe changes
	FOdysseySurfaceEditable*            mDisplaySurface;

	//Surface for previewing in viewport
	FOdysseySurfaceReadOnly*            mPreviewSurface;

    //Current Texture Properties backup
    FTexturePropertiesBackup    mPropertiesBackup;
};

