// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "IOdysseyTextureEditorModule.h"

#include "IAssetTypeActions.h"

class FOdysseyTextureEditorModule
    : public IOdysseyTextureEditorModule
{
public:
    // IOdysseyTextureEditorModule interface
    virtual void CreateOdysseyTextureEditor( TArray<UTexture2D*> iTextures ) override;

public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    //Asset Type Action
    void RegisterAssetTypeActions();
    void UnregisterAssetTypeActions();

    //Settings
    void RegisterSettings();
    void UnregisterSettings();

    //Commands
    void RegisterCommands();
    void UnregisterCommands();

    //Level Editor Layout
    void RegisterLevelEditorLayoutExtensions();
    void UnregisterLevelEditorLayoutExtensions();

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TSharedPtr<IAssetTypeActions> mOdysseyTypeActions;
    FDelegateHandle mExtendLevelEditorLayout;

    /** Holds the menu extensibility manager. */
    TSharedPtr<FExtensibilityManager> mMenuExtensibilityManager;
};
