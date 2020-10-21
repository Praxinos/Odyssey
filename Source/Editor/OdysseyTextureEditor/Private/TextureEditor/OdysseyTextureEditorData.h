// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorData.h"

class UTexture;
class UOdysseyBrush;

struct FTexturePropertiesBackup
{
    TextureMipGenSettings       mTextureMipGenBackup;
    TextureCompressionSettings  mTextureCompressionBackup;
    TextureGroup                mTextureGroupBackup;
	FTextureFormatSettings      mTextureFormatSettings;
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

private:
    UTexture2D*                 mTexture;
    FOdysseyLayerStack*         mLayerStack;        // Copied from mOdysseyTexture AssetUserData
	FOdysseySurfaceEditable*    mDisplaySurface;

    FTexturePropertiesBackup    mPropertiesBackup;
};

