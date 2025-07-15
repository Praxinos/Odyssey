// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FAssetTypeActions_OdysseyPalette;
class IAssetTypeActions;

/**
 * The Texture Asset module.
 */
class FOdysseyPaletteModule :
    public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

private:
    //Asset Type Action
    void RegisterAssetTypeActions();
    void UnregisterAssetTypeActions();

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<IAssetTypeActions> mOdysseyTypeActions;

};
