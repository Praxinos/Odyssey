// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureModule.h"

#include "AssetToolsModule.h"
#include "Settings/ContentBrowserSettings.h"

#include "OdysseyTexture_AssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureModule"

TSharedPtr<FExtensibilityManager> FOdysseyTextureModule::GetMenuExtensibilityManager()
{
	return mMenuExtensibilityManager;
}

void FOdysseyTextureModule::RegisterAssetTypeAction(IAssetTools& ioAssetTools, TSharedRef<IAssetTypeActions> iAction)
{
	ioAssetTools.RegisterAssetTypeActions(iAction);
	mCreatedAssetTypeActions.Add(iAction);
}

// From ...\UnrealEngine\Engine\Source\Editor\ContentBrowser\Private\SAssetView.cpp#3543
void FOdysseyTextureModule::ShowPluginContentInContentBrowser()
{
	if (GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders())
		return;

	bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
	bool bRawDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders(true);

	// Only if both these flags are false when toggling we want to enable the flag, otherwise we're toggling off
	if (!bDisplayPlugins && !bRawDisplayPlugins)
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(true);
	}
	else
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(false);
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(false, true);
	}
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

void FOdysseyTextureModule::StartupModule() {
	// Register asset types
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	mOdysseyPainterCategory = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));
	RegisterAssetTypeAction(assetTools, MakeShareable(new FOdysseyTextureAssetTypeActions(mOdysseyPainterCategory)));

	// register menu extensions
	mMenuExtensibilityManager = MakeShareable(new FExtensibilityManager);

	ShowPluginContentInContentBrowser();

	if (!IsRunningCommandlet())
	{
		FOdysseyTextureContentBrowserExtensions::InstallHooks();
	}
}

void FOdysseyTextureModule::ShutdownModule()
{

	// unregister menu extensions
	mMenuExtensibilityManager.Reset();

	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 index = 0; index < mCreatedAssetTypeActions.Num(); ++index)
		{
			assetTools.UnregisterAssetTypeActions(mCreatedAssetTypeActions[index].ToSharedRef());
		}
	}

	FOdysseyTextureContentBrowserExtensions::RemoveHooks();
}

IMPLEMENT_MODULE(FOdysseyTextureModule, OdysseyTexture);