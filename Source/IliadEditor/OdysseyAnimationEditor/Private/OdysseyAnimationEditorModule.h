// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "Modules/ModuleManager.h"
#include "IAssetTypeActions.h"

class FOdysseyAnimationEditorModule
    : public IModuleInterface
{
public:
    // IOdysseyAnimationEditorModule interface
    //virtual TSharedRef<FOdysseyAnimationEditorToolkit> CreateOdysseyAnimationEditor( UOdysseyAnimation* iAnimation ) override;

public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    //Asset Type Action
    void RegisterAssetTypeActions();
    void UnregisterAssetTypeActions();

    void RegisterFactoryDelayed();
    void RegisterPlacementFactories();
    void UnregisterPlacementFactories();
    void ReorderFactories();

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<IAssetTypeActions> mOdysseyTypeActions;
};
