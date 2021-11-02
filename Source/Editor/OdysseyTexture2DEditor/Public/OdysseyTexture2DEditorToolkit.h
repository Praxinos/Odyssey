// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyAssetEditorToolkit.h"

#include <ULIS>

class FOdysseyTexture2DEditorData;
class FOdysseyTexture2DEditorGUI;
class FOdysseyTexture2DEditorController;

class UTexture;

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyTexture2DEditorToolkit
    : public FOdysseyAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTexture2DEditorToolkit();
    FOdysseyTexture2DEditorToolkit(TSharedPtr<FOdysseyTexture2DEditor> iEditor);

protected:
	virtual void OpenAsset(UObject* iObject) override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
};

