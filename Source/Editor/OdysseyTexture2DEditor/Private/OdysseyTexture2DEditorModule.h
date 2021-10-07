// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once

#include "IOdysseyTexture2DEditorModule.h"

#include "IAssetTypeActions.h"

class FOdysseyTexture2DEditorModule
	: public IOdysseyTexture2DEditorModule
{
public:
	// IOdysseyTexture2DEditorModule interface
	virtual TSharedRef<FOdysseyTexture2DEditorToolkit> CreateOdysseyTexture2DEditor( UTexture2D* iTexture ) override;

public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

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
	TSharedPtr<IAssetTypeActions> mIliadTypeActions;
};
