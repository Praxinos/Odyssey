// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.


#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Interfaces/IOdysseyPainterEditorToolkit.h"
#include "Interfaces/IOdysseyPainterEditorModule.h"
#include "PropertyEditorModule.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorToolkit.h"
#include "ISettingsModule.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy_ContentBrowserExtensions.h"
#include "AssetToolsModule.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy_AssetTypeActions.h"
#include "OdysseyBaseTests.h"
#include "LevelEditor.h"


#define LOCTEXT_NAMESPACE "OdysseyPainterEditorModule"




/*-----------------------------------------------------------------------------
   FOdysseyPainterEditorModule
-----------------------------------------------------------------------------*/

class FOdysseyPainterEditorModule
    : public IOdysseyPainterEditorModule
{
public:

    // IOdysseyPainterEditorModule interface
    virtual TSharedRef<IOdysseyPainterEditorToolkit> CreateOdysseyPainterEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UTexture2D* InTexture ) override
    {
        TSharedRef<FOdysseyPainterEditorToolkit> NewOdysseyPainterEditor(new FOdysseyPainterEditorToolkit());
        NewOdysseyPainterEditor->InitOdysseyPainterEditor( Mode, InitToolkitHost, InTexture );

        return NewOdysseyPainterEditor;
    }

    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager( ) override
    {
        return MenuExtensibilityManager;
    }

private:
    void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
    {
        AssetTools.RegisterAssetTypeActions(Action);
        CreatedAssetTypeActions.Add(Action);
    }

public:

    // IModuleInterface interface

    virtual void StartupModule( ) override
    {
        //TMP: Run Tests
        FOdysseyBaseTests::RunTests();

        // Register asset types

        IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

        OdysseyPainterCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

        RegisterAssetTypeAction(AssetTools, MakeShareable(new FOdysseyTextureAssetTypeActions(OdysseyPainterCategory)));


        // register menu extensions
        MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);

        // register settings
        ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

        if (SettingsModule != nullptr)
        {
            SettingsModule->RegisterSettings("Editor", "ContentEditors", "ILIADPainterEditor",
                LOCTEXT("OdysseyPainterEditorSettingsName", "ILIAD Painter Editor"),
                LOCTEXT("OdysseyPainterEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor."),
                GetMutableDefault<UOdysseyPainterEditorSettings>()
            );
        }

        if (!IsRunningCommandlet())
        {
            FOdysseyPainterContentBrowserExtensions::InstallHooks();
        }
    }

    virtual void ShutdownModule( ) override
    {
        // unregister settings
        ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

        if (SettingsModule != nullptr)
        {
            SettingsModule->UnregisterSettings("Editor", "ContentEditors", "OdysseyPainterEditor");
        }

        // unregister menu extensions
        MenuExtensibilityManager.Reset();

        // Unregister all the asset types that we registered
        if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
        {
            IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
            for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
            {
                AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
            }
        }
    }

private:

    /** All created asset type actions.  Cached here so that we can unregister them during shutdown. */
    TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

    EAssetTypeCategories::Type OdysseyPainterCategory;

    // Holds the menu extensibility manager.
    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
};


IMPLEMENT_MODULE(FOdysseyPainterEditorModule, OdysseyPainterEditor);


#undef LOCTEXT_NAMESPACE
