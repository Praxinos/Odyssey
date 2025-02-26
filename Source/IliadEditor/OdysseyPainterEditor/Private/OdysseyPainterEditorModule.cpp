// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorModule.h"

#include "FOdysseyVectorBrushCustomization.h"
#include "FOdysseyVectorObjectViewPaletteCustomization.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "OdysseyPainterEditorSettings.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyBrushAssetBase.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingToolOverrides.h"

#include "PainterEditor/OdysseyPainterEditorGUI.h"
#include "StandaloneEditor/OdysseyPainterEditorStandaloneToolkit.h"
#include <ULIS>

#define LOCTEXT_NAMESPACE "PainterEditor"

/*-----------------------------------------------------------------------------
   FOdysseyPainterEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyPainterEditorModule::OpenStandaloneEditorForAsset( UObject* iAsset )
{
    if (!iAsset)
        return;

    /* TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
        editorId,
        editorName,
        iAsset,
        editorLayoutName
    ); */

    /* TSharedRef<FOdysseyAnimationEditorExtension> animationExtension = MakeShared<FOdysseyAnimationEditorExtension>(editor.Get());
    editor->AddExtension(animationExtension); */

    TSharedRef<FOdysseyPainterEditorStandaloneToolkit> toolkit = MakeShared<FOdysseyPainterEditorStandaloneToolkit>(iAsset);
    toolkit->Open();

    //-----

    /* TSharedPtr<FOdysseyAnimationEditorSource> source = MakeShared<FOdysseyAnimationEditorSource>(iAnimation);
    editor->SetSource(source); */
}

void
FOdysseyPainterEditorModule::StartupModule()
{
    RegisterBrushOverrides(); //First thing to do, as it modifies the Brush CDO
    RegisterSettings();
    RegisterCommands();
    RegisterLevelEditorLayoutExtensions();
    RegisterDetailCustomizations();

    FOdysseyVectorBrushCustomization::Register();
    FOdysseyVectorObjectViewPaletteCustomization::Register();
}

void
FOdysseyPainterEditorModule::ShutdownModule()
{
    UnregisterBrushOverrides();
    UnregisterSettings();
    UnregisterCommands();
    UnregisterLevelEditorLayoutExtensions();
    UnregisterDetailCustomization();

    FOdysseyVectorBrushCustomization::Unregister();
    FOdysseyVectorObjectViewPaletteCustomization::Unregister();

    //---
    for (const auto& element : mOpenedTabIds)
    {
        const FName& editorName = element.Key;
        SaveOpenedTabIds(editorName);
    }
}

void
FOdysseyPainterEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditor"
                                        , LOCTEXT( "settings.name", "Odyssey Painter Editor" )
                                        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the Odyssey Editor." )
                                        , GetMutableDefault<UOdysseyPainterEditorSettings>() );
}

void
FOdysseyPainterEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditor" );
}

void
FOdysseyPainterEditorModule::RegisterCommands()
{
    FOdysseyPainterEditorCommands::Register();
}

void
FOdysseyPainterEditorModule::UnregisterCommands()
{
    FOdysseyPainterEditorCommands::Unregister();
}

void
FOdysseyPainterEditorModule::RegisterBrushOverrides()
{
    FOdysseyBrushOverride::Register(UOdysseyBrushOptionsOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyBlendParametersOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyFreehandShapeOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyPainterEditorRasterDrawingToolOverrides::StaticClass());
}

void
FOdysseyPainterEditorModule::UnregisterBrushOverrides()
{

}

void
FOdysseyPainterEditorModule::RegisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyPainterEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyPainterEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
}

void
FOdysseyPainterEditorModule::RegisterDetailCustomizations()
{
    FOdysseyShapes::RegisterDetailCustomization();
}

void
FOdysseyPainterEditorModule::UnregisterDetailCustomization()
{
    FOdysseyShapes::UnregisterDetailCustomization();
}

void
FOdysseyPainterEditorModule::SetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iTabIds)
{
    TArray<FName>& tabIds = mOpenedTabIds.FindOrAdd(iEditorName);
    tabIds = iTabIds;
}

const TArray<FName>&
FOdysseyPainterEditorModule::GetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds)
{
    if (!mOpenedTabIds.Contains(iEditorName))
        LoadOpenedTabIds(iEditorName, iDefaultOpenedTabIds);

    return mOpenedTabIds[iEditorName];
}

FString
FOdysseyPainterEditorModule::GetOpenedTabIdsSavedPath() const
{
    FString filename = FApp::GetProjectName() + FString("OdysseyLayout.ini");
    return FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetEpicProductIdentifier(), TEXT("Editor"), TEXT("Odyssey"), filename);
}

FString
FOdysseyPainterEditorModule::GetOpenedTabIdsProjectPath() const
{
    FString filename = "OdysseyLayout.ini";
    return FPaths::Combine(FPaths::ProjectConfigDir(), filename);
}

void
FOdysseyPainterEditorModule::LoadOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds)
{
    FString savedPath = GetOpenedTabIdsSavedPath();
    FString projectPath = GetOpenedTabIdsProjectPath();

    TArray<FName>& tabIds = mOpenedTabIds.FindOrAdd(iEditorName);

    FConfigFile* configFile = GConfig->Find(savedPath);
    if ( !configFile || !configFile->Contains(iEditorName.ToString()) )
    {
        configFile = GConfig->Find(projectPath);

        if ( !configFile || !configFile->Contains(iEditorName.ToString()) )
        {
            tabIds = iDefaultOpenedTabIds;
            return;
        }
    }

    TArray<FString> tabStringIds;
    configFile->GetArray(
        *iEditorName.ToString(),
        TEXT("OpenedTabs"),
        tabStringIds);

    tabIds.Empty();
    for ( const FString& tabId : tabStringIds )
    {
        tabIds.Add(FName(tabId));
    }
}

void
FOdysseyPainterEditorModule::SaveOpenedTabIds(const FName& iEditorName)
{
    FString savedPath = GetOpenedTabIdsSavedPath();
    TArray<FName>& tabIds = mOpenedTabIds.FindOrAdd(iEditorName);

    TArray<FString> tabStringIds;
    for ( const FName& tabId : tabIds )
    {
        tabStringIds.Add(tabId.ToString());
    }

    GConfig->SetArray(
        *iEditorName.ToString(),
        TEXT("OpenedTabs"),
        tabStringIds,
        savedPath);
}

IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
