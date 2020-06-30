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
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	
	// End of IModuleInterface interface

	void ShowPluginContentInContentBrowser();
};
