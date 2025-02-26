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
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyBrushAssetBase.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingToolOverrides.h"
#include "ActorFactories/ActorFactory.h"
#include "EditorModeRegistry.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "Interfaces/IPluginManager.h"
#include "OdysseyViewportDrawingEditorCommands.h"

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationCellThumbnailRenderer.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationEditorGUI.h"
#include "OdysseyAnimationEditorProjectSettings.h"
#include "OdysseyAnimationEditorUserSettings.h"
#include "OdysseyAnimationEditorCommands.h"
#include "Texture/OdysseyTextureEditorGUI.h"
#include "OdysseyFlipbookEditorCommands.h"
#include "OdysseyFlipbookEditorGUI.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "StandaloneEditor/OdysseyPainterEditorStandaloneToolkit.h"

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
    RegisterThumbnailRenderers();
    RegisterEditorMode();
    RegisterShaders();
    RegisterPropertyModuleCustomizations();

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
    UnregisterThumbnailRenderers();
    UnregisterEditorMode();
    UnregisterShaders();
    UnregisterPropertyModuleCustomizations();

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
FOdysseyPainterEditorModule::RegisterEditorMode()
{
    FEditorModeRegistry::Get().RegisterMode<FOdysseyViewportDrawingEditorEdMode>(
        FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId,
        LOCTEXT("editor-mode.name", "Odyssey"),
        FSlateIcon(FOdysseyStyle::GetStyleSetName(), "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon40", "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon16"),
        true, 200 );
}

void
FOdysseyPainterEditorModule::UnregisterEditorMode()
{
    FEditorModeRegistry::Get().UnregisterMode(FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId);
}

void
FOdysseyPainterEditorModule::RegisterShaders()
{
    FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Odyssey"))->GetBaseDir(),TEXT("Shaders"));
    AddShaderSourceDirectoryMapping(TEXT("/Plugin/Odyssey"),PluginShaderDir);
}

void
FOdysseyPainterEditorModule::RegisterPropertyModuleCustomizations()
{
    /** Register detail/property customization */
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FModuleManager::Get().LoadModule("MeshPaint");
}

void
FOdysseyPainterEditorModule::UnregisterPropertyModuleCustomizations()
{
    /** De-register detail/property customization */
    FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
    if (PropertyModule)
    {
        PropertyModule->UnregisterCustomClassLayout("OdysseyViewportDrawingEditorSettings");
        PropertyModule->UnregisterCustomPropertyTypeLayout("OdysseyViewportDrawingEditorTexturePaintSettings");
    }
}

void
FOdysseyPainterEditorModule::UnregisterShaders()
{
    //No method available to unregister Shaders directories
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

    settingsModule->RegisterSettings( "Project", "Plugins", "OdysseyAnimationEditor"
        , LOCTEXT( "settings.name", "2D Animation Editor" )
        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the 2D Animation Editor." )
        , GetMutableDefault<UOdysseyAnimationEditorProjectSettings>() );

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyAnimationEditorUserSettings"
        , LOCTEXT( "settings.name", "2D Animation Editor" )
        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the 2D Animation Editor." )
        , GetMutableDefault<UOdysseyAnimationEditorUserSettings>() );
}

void
FOdysseyPainterEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditorUserSettings" );
}

void
FOdysseyPainterEditorModule::RegisterCommands()
{
    FOdysseyPainterEditorCommands::Register();
    FOdysseyAnimationEditorCommands::Register();
    FOdysseyFlipbookEditorCommands::Register();
    FOdysseyViewportDrawingEditorCommands::Register();
}

void
FOdysseyPainterEditorModule::UnregisterCommands()
{
    FOdysseyPainterEditorCommands::Unregister();
    FOdysseyAnimationEditorCommands::Unregister();
    FOdysseyFlipbookEditorCommands::Unregister();
    FOdysseyViewportDrawingEditorCommands::Unregister();
}

void
FOdysseyPainterEditorModule::RegisterThumbnailRenderers()
{
    UThumbnailManager::Get().RegisterCustomRenderer(UOdysseyAnimationCell::StaticClass(), UOdysseyAnimationCellThumbnailRenderer::StaticClass());
}

void
FOdysseyPainterEditorModule::UnregisterThumbnailRenderers()
{
    //UThumbnailManager::Get().UnregisterCustomRenderer(UOdysseyAnimationCellImageRaster::StaticClass());
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
    mAnimationExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyAnimationEditorGUI::ExtendLevelEditorLayout);
    mTextureExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyTextureEditorGUI::ExtendLevelEditorLayout);
    mFlipbookExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyFlipbookEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyPainterEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mAnimationExtendLevelEditorLayout);
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mTextureExtendLevelEditorLayout);
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mFlipbookExtendLevelEditorLayout);
}

void
FOdysseyPainterEditorModule::RegisterDetailCustomizations()
{
    FOdysseyShapes::RegisterDetailCustomization();

    FOdysseyAnimationEditorFlipSystem::RegisterDetailCustomization();
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyAnimationEditorOutOfPegsTool::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyAnimationEditorOutOfPegsToolDetails::MakeInstance));
}

void
FOdysseyPainterEditorModule::UnregisterDetailCustomization()
{
    FOdysseyShapes::UnregisterDetailCustomization();
    FOdysseyAnimationEditorFlipSystem::UnregisterDetailCustomization();
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
