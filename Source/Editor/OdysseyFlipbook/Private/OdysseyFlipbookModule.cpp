// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookModule.h"
#include "AssetToolsModule.h"
#include "Settings/ContentBrowserSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookModule"

// From ...\UnrealEngine\Engine\Source\Editor\ContentBrowser\Private\SAssetView.cpp#3543
void FOdysseyFlipbookModule::ShowPluginContentInContentBrowser()
{
	if (GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders())
		return;

	bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();

	// Only if both these flags are false when toggling we want to enable the flag, otherwise we're toggling off
	if (!bDisplayPlugins)
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(true);
	}
	else
	{
		GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(false);
	}
	GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
}

void FOdysseyFlipbookModule::StartupModule()
{
	ShowPluginContentInContentBrowser();
}

void FOdysseyFlipbookModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FOdysseyFlipbookModule, OdysseyFlipbook);

#undef LOCTEXT_NAMESPACE
