// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAssetEditorToolkit.h"

#include <ULIS>

class FOdysseyPainterEditor;

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
    FOdysseyTextureEditorToolkit();

protected:
    virtual void OpenAsset(UObject* iObject) override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
};

