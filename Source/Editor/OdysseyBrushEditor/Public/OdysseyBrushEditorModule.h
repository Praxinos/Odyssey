// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
