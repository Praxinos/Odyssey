// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyToolkit.h"

#include "Toolkits/AssetEditorToolkit.h"

/**
 * Implements an Editor toolkit for the Painter Editor.
 * The toolkit is the main entry point for the Painter Editor
 */
class ODYSSEYEDITOR_API FOdysseyAssetEditorToolkit
    : public TOdysseyToolkit<FAssetEditorToolkit>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAssetEditorToolkit();
    FOdysseyAssetEditorToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor);

public:
    virtual void Initialize() override;

protected:
    // FAssetEditorToolkit interface
    virtual void SaveAssetAs_Execute() override;
    virtual bool OnRequestClose() override;
    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;

	/** Called to check to see if there's an asset capable of being reimported */
	virtual bool CanReimport() const;
	virtual bool CanReimport(UObject* EditingObject) const;

protected:
	virtual void OpenAsset(UObject* iObject) = 0;

private:
	virtual void ExtendMenu() override;
    void OnAddEditedObject(UObject* iObject);
    void OnRemoveEditedObject(UObject* iObject);
};
