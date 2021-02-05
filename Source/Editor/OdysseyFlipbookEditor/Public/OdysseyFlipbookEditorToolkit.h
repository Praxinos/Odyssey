// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorToolkit.h"

#include <ULIS3>

class FOdysseyFlipbookEditorData;
class FOdysseyFlipbookEditorGUI;
class FOdysseyFlipbookEditorController;

class UPaperFlipbook;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class FOdysseyFlipbookEditorToolkit
    : public FOdysseyPainterEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorToolkit();
    FOdysseyFlipbookEditorToolkit();

protected:
	virtual void OpenAsset(UObject* iObject) override;
    virtual TArray<UObject*> GetAllEditedObjects() override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
};

