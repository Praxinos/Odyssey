// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	
	// End of IModuleInterface interface

	void ShowPluginContentInContentBrowser();
};
