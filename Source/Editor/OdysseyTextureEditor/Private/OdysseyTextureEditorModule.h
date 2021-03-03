// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyTextureEditorModule.h"

class FOdysseyTextureEditorModule
	: public IOdysseyTextureEditorModule
{

public:
	// IOdysseyTextureEditorModule interface
    virtual TSharedRef<FOdysseyTextureEditorToolkit> CreateOdysseyTextureEditor( UTexture2D* iTexture ) override;

    // IModuleInterface interface
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;

public:
	//Getters
	TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override;

private:
	//Asset Type Action
    void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();

	//Settings
	void RegisterSettings();
	void UnregisterSettings();

	//Commands
	void RegisterCommands();
	void UnregisterCommands();
    
private:
	/** All created asset type actions. Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > mTypeActions;

	// Holds the menu extensibility manager.
	TSharedPtr<FExtensibilityManager> mMenuExtensibilityManager;
};
