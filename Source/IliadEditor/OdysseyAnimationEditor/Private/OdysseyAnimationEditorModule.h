// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "Modules/ModuleManager.h"
#include "IAssetTypeActions.h"

class FOdysseyAnimationEditorModule
    : public IModuleInterface
{
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

    void RegisterPropertyCustomizations();
    void UnregisterPropertyCustomizations();

    void OnPostEngineInit();
    void OnLevelActorAdded(AActor* iActor);
private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<IAssetTypeActions> mOdysseyTypeActions;
};
