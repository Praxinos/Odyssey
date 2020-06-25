// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class  FAssetTypeActions_OdysseyTexture;

/**
 * The Texture Asset module.
 */
class FOdysseyTextureModule :
	public IModuleInterface
	, public IHasMenuExtensibility
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	
	TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override;
	// End of IModuleInterface interface

	void ShowPluginContentInContentBrowser();

	void RegisterAssetTypeAction(IAssetTools& ioAssetTools, TSharedRef<IAssetTypeActions> iAction);

    TSharedPtr< FAssetTypeActions_OdysseyTexture >  OdysseyTextureAssetTypeActions;

private:
	/** All created asset type actions. Cached here so that we can unregister them during shutdown. */
	TArray< TSharedPtr<IAssetTypeActions> > mCreatedAssetTypeActions;

	EAssetTypeCategories::Type mOdysseyPainterCategory;

	// Holds the menu extensibility manager.
	TSharedPtr<FExtensibilityManager> mMenuExtensibilityManager;
};
