// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once

#include "IOdysseyFlipbookEditorModule.h"

class FOdysseyFlipbookEditorModule
	: public IOdysseyFlipbookEditorModule
{
public:
	// IOdysseyFlipbookEditorModule interface
    virtual TSharedRef<FOdysseyFlipbookEditorToolkit> CreateOdysseyFlipbookEditor( UPaperFlipbook* iFlipbook ) override;

    // IModuleInterface interface
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;

	// AssetTypeActions
    void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

private:

	// Settings
	void RegisterSettings();
	void UnregisterSettings();

    // Commands
	void RegisterCommands();
	void UnregisterCommands();

private:
	/** All created asset type actions. Cached here so that we can unregister them during shutdown. */
	TSharedPtr<IAssetTypeActions> mIliadTypeActions;
    // AssetTypeActions that don't belong to us, we keep track of them to be able to (un)register them, based on Iliad preferences
    TSharedPtr<IAssetTypeActions> mUETypeActions;
};
