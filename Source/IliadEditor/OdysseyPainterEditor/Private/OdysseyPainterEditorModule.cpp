// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorModule.h"

#include "FOdysseyVectorBrushCustomization.h"
#include "FOdysseyVectorObjectViewPaletteCustomization.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyBrushOptionsOverrides.h"
#include "OdysseyBlendParametersOverrides.h"
#include "OdysseyFreehandShapeOverrides.h"
#include "OdysseyPainterEditorRasterDrawingToolOverrides.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyPainterEditorRasterLiquifyTool.h"
#include "ActorFactories/ActorFactory.h"
#include "EditorModeRegistry.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "Interfaces/IPluginManager.h"
#include "OdysseyViewportDrawingEditorCommands.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationCellThumbnailRenderer.h"
#include "OdysseyPainterEditorLevelEditorLayout.h"
#include "OdysseyPainterEditorAnimationProjectSettings.h"
#include "OdysseyPainterEditorAnimationUserSettings.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditorTextureCommands.h"
#include "OdysseyPainterEditorFlipbookCommands.h"
#include "OdysseyToolCollectionAssetTypeActions.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "StandaloneEditor/OdysseyPainterEditorStandaloneToolkit.h"
#include "OdysseyTelemetryModule.h"
#include "OdysseyViewportCommands.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/*-----------------------------------------------------------------------------
   FOdysseyPainterEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyPainterEditorModule::OpenStandaloneEditorForAsset( UObject* iAsset )
{
    if (!iAsset)
        return;

    TSharedRef<FOdysseyPainterEditorStandaloneToolkit> toolkit = MakeShared<FOdysseyPainterEditorStandaloneToolkit>(iAsset);
    toolkit->Open();
}

void
FOdysseyPainterEditorModule::AddOpenedEditor(FOdysseyPainterEditor* iEditor)
{
    mOpenedEditors.AddUnique(iEditor);
}

void
FOdysseyPainterEditorModule::RemoveOpenedEditor(FOdysseyPainterEditor* iEditor)
{
    mOpenedEditors.Remove(iEditor);
}

FOdysseyPainterEditor*
FOdysseyPainterEditorModule::GetOpenedEditorForAsset(UObject* iObject)
{
    for (FOdysseyPainterEditor* editor : mOpenedEditors)
    {
        UObject* editedObject = editor->GetEditedObject();
        if (editedObject == iObject)
            return editor;

        TArray<UObject*> additionalEditedObjects = editor->GetAdditionalEditedObjects();
        if (additionalEditedObjects.Contains(iObject))
            return editor;
    }

    return nullptr;
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
    RegisterPropertyModuleCustomizations();
    RegisterAssetTypeActions();
    RegisterTelemetry();

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
    UnregisterPropertyModuleCustomizations();
    UnregisterAssetTypeActions();
    UnregisterTelemetry();

    FOdysseyVectorBrushCustomization::Unregister();
    FOdysseyVectorObjectViewPaletteCustomization::Unregister();
}

void
FOdysseyPainterEditorModule::RegisterEditorMode()
{
    FEditorModeRegistry::Get().RegisterMode<FOdysseyViewportDrawingEditorEdMode>(
        FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId,
        LOCTEXT("editor-mode.name", "Odyssey"),
        FSlateIcon(FOdysseyStyle::GetStyleSetName(), "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon40", "OdysseyViewportDrawingEditMode.OdysseyViewportDrawingIcon16"),
        true );
}

void
FOdysseyPainterEditorModule::UnregisterEditorMode()
{
    FEditorModeRegistry::Get().UnregisterMode(FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId);
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
FOdysseyPainterEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditor"
        , LOCTEXT( "user-settings.painter-editor.name", "Odyssey - Painter Editor" )
        , LOCTEXT( "user-settings.painter-editor.tooltip", "Configure the look and feel of the Odyssey Editor." )
        , GetMutableDefault<UOdysseyPainterEditorSettings>() );

    settingsModule->RegisterSettings( "Project", "Plugins", "OdysseyAnimationEditor"
        , LOCTEXT( "project-settings.2d-animation-editor.name", "Odyssey - 2D Animation" )
        , LOCTEXT( "project-settings.2d-animation-editor.tooltip", "Configure the look and feel of the 2D Animation Editor." )
        , GetMutableDefault<UOdysseyPainterEditorAnimationProjectSettings>() );

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditorAnimationUserSettings"
        , LOCTEXT( "user-settings.2d-animation-editor.name", "Odyssey - 2D Animation Editor" )
        , LOCTEXT( "user-settings.2d-animation-editor.tooltip", "Configure the look and feel of the 2D Animation Editor." )
        , GetMutableDefault<UOdysseyPainterEditorAnimationUserSettings>() );
}

void
FOdysseyPainterEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditorAnimationUserSettings" );
}

void
FOdysseyPainterEditorModule::RegisterCommands()
{
    FOdysseyPainterEditorCommands::Register();
    FOdysseyPainterEditorAnimationCommands::Register();
    FOdysseyPainterEditorTextureCommands::Register();
    FOdysseyPainterEditorFlipbookCommands::Register();
    FOdysseyViewportDrawingEditorCommands::Register();
}

void
FOdysseyPainterEditorModule::UnregisterCommands()
{
    FOdysseyPainterEditorCommands::Unregister();
    FOdysseyPainterEditorAnimationCommands::Unregister();
    FOdysseyPainterEditorTextureCommands::Unregister();
    FOdysseyPainterEditorFlipbookCommands::Unregister();
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
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyPainterEditorLevelEditorLayout::ExtendLevelEditorLayout);
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
    UOdysseyPainterEditorRasterDrawingTool::RegisterDetailCustomization();
    UOdysseyPainterEditorRasterLiquifyTool::RegisterDetailCustomization();

    FOdysseyPainterEditorAnimationFlipSystem::RegisterDetailCustomization();
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyPainterEditorAnimationOutOfPegsTool::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyPainterEditorAnimationOutOfPegsToolDetails::MakeInstance));
}

void
FOdysseyPainterEditorModule::UnregisterDetailCustomization()
{
    UOdysseyPainterEditorRasterDrawingTool::UnregisterDetailCustomization();
    UOdysseyPainterEditorRasterLiquifyTool::UnregisterDetailCustomization();
    FOdysseyShapes::UnregisterDetailCustomization();
    FOdysseyPainterEditorAnimationFlipSystem::UnregisterDetailCustomization();
}

void
FOdysseyPainterEditorModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("Odyssey")), LOCTEXT("asset-category.name", "Odyssey"));

    //Create Asset Types Actions
    mOdysseyTypeActions = MakeShareable(new FOdysseyToolCollectionAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyPainterEditorModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

//---

void
FOdysseyPainterEditorModule::RegisterTelemetry()
{
    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UOdysseyToolCollection::StaticClass() );
}

void
FOdysseyPainterEditorModule::UnregisterTelemetry()
{
    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UOdysseyToolCollection::StaticClass() );
}


IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
