// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureModule.h"

#include "AssetToolsModule.h"
#include "Settings/ContentBrowserSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureModule"

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

void FOdysseyTextureModule::StartupModule()
{
	ShowPluginContentInContentBrowser();
}

void FOdysseyTextureModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FOdysseyTextureModule, OdysseyTexture);

#undef LOCTEXT_NAMESPACE
