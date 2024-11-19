// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyAssetEditorToolkit.h"

#include <ULIS>

class FOdysseyFlipbookEditorData;
class FOdysseyFlipbookEditorGUI;
class FOdysseyFlipbookEditorController;

class UPaperFlipbook;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class FOdysseyFlipbookEditorToolkit
    : public FOdysseyAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorToolkit();
    FOdysseyFlipbookEditorToolkit();

protected:
    virtual void OpenAsset(UObject* iObject) override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
};
