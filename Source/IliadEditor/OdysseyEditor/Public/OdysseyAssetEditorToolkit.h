// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class FOdysseyEditor;

/**
 * Implements an Editor toolkit for the Painter Editor.
 * The toolkit is the main entry point for the Painter Editor
 */
class ODYSSEYEDITOR_API FOdysseyAssetEditorToolkit
    : public FAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAssetEditorToolkit();
    FOdysseyAssetEditorToolkit(const FName& iAppIdentifier);

public:
    void Initialize(UObject* iEditedObject, TSharedPtr<FOdysseyEditor> iEditor);

protected:
    // FAssetEditorToolkit interface
    virtual void SaveAssetAs_Execute() override;
    virtual bool OnRequestClose() override;
    virtual void OnClose() override;
    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;

    /** Called to check to see if there's an asset capable of being reimported */
    virtual bool CanReimport() const;
    virtual bool CanReimport(UObject* EditingObject) const;

protected:
    virtual void OpenAsset(UObject* iObject) = 0;

private:
    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);

private:
    FName mAppIdentifier;
    TSharedPtr<FOdysseyEditor> mEditor;
};
