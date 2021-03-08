// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyAssetEditorToolkit.h"

#include <ULIS3>

class FOdysseyTextureEditorData;
class FOdysseyTextureEditorGUI;
class FOdysseyTextureEditorController;

class UTexture;

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyTextureEditorToolkit
    : public FOdysseyAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorToolkit();
    FOdysseyTextureEditorToolkit(TSharedPtr<FOdysseyTextureEditor> iEditor);

protected:
	virtual void OpenAsset(UObject* iObject) override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
};

