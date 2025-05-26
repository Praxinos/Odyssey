// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "Modules/ModuleManager.h"

class IAssetTypeActions;

class FOdysseyFlipbookEditorModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    // AssetTypeActions
    void RegisterAssetTypeActions();
    void UnregisterAssetTypeActions();

private:
    // Settings
    void RegisterSettings();
    void UnregisterSettings();

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<IAssetTypeActions> mOdysseyTypeActions;
    // AssetTypeActions that don't belong to us, we keep track of them to be able to (un)register them, based on Odyssey preferences
    TSharedPtr<IAssetTypeActions> mUETypeActions;
};
