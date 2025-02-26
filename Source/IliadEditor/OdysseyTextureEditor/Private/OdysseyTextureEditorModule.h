// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "Modules/ModuleManager.h"
#include "IAssetTypeActions.h"

class FOdysseyTextureEditorModule
    : public IModuleInterface
{
public:
    // IOdysseyTextureEditorModule interface
    //void CreateOdysseyTextureEditor( TArray<UTexture2D*> iTextures );

public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    //Asset Type Action
    void RegisterAssetTypeActions();
    void UnregisterAssetTypeActions();

    // Settings
    void RegisterSettings();
    void UnregisterSettings();

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<IAssetTypeActions> mOdysseyTypeActions;
};
