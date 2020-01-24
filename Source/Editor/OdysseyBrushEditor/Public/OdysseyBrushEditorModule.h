// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Framework/Commands/UICommandList.h"
#include "Toolkits/AssetEditorToolkit.h"

class  FAssetTypeActions_OdysseyBrush;

/**
 * The Brush editor module provides the brush editor application via classical bp editor.
 */
class FOdysseyBrushEditorModule : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

    TSharedPtr< FAssetTypeActions_OdysseyBrush >  OdysseyBrushAssetTypeActions;
};
