// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "Modules/ModuleManager.h"

class FOdysseyFlipbookAssetTypeActions;

class FOdysseyFlipbookEditorModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    TSharedPtr<FOdysseyFlipbookAssetTypeActions> GetFlipbookTypeActions() const;

private:
    // Settings
    void RegisterSettings();
    void UnregisterSettings();

    // AssetTypeActions
    void RegisterAssetTypeActions();
    void UnregisterAssetTypeActions();

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<FOdysseyFlipbookAssetTypeActions> mFlipbookTypeActions;
};
