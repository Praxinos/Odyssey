// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "OdysseyAnimationEditorToolkit.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorProjectSettings.h"
#include "AnimationEditor/OdysseyAnimationEditorUserSettings.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyAnimation.h"
#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "OdysseyAnimationEditorFlipSystem.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCellThumbnailRenderer.h"
#include "ISequencerModule.h"
#include "ILevelSequenceModule.h"


#define LOCTEXT_NAMESPACE "AnimationEditor"

/*-----------------------------------------------------------------------------
   FOdysseyAnimationEditorModule
-----------------------------------------------------------------------------*/

TSharedRef<FOdysseyAnimationEditorToolkit>
FOdysseyAnimationEditorModule::CreateOdysseyAnimationEditor( UOdysseyAnimation* iAnimation )
{
    TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
        TEXT("OdysseyAnimationEditor"),
        LOCTEXT("main-menu.category", "Odyssey Animation Editor"),
        iAnimation,
        "OdysseyAnimationEditor_Layout"
    );

    TSharedRef<FOdysseyAnimationEditorExtension> animationExtension = MakeShared<FOdysseyAnimationEditorExtension>(editor.Get());
    editor->AddExtension(animationExtension);

    TSharedPtr<FOdysseyAnimationEditorToolkit> toolkit = MakeShared<FOdysseyAnimationEditorToolkit>();
    toolkit->Initialize(iAnimation, editor);
    //-----

    TSharedPtr<FOdysseyAnimationEditorSource> source = MakeShared<FOdysseyAnimationEditorSource>(iAnimation);
    editor->SetSource(source);

    return toolkit.ToSharedRef();
}

void
FOdysseyAnimationEditorModule::StartupModule()
{
    RegisterAssetTypeActions();

    // Register Commands
    RegisterCommands();

    RegisterSettings();

    RegisterLevelEditorLayoutExtensions();

    RegisterDetailCustomizations();

    RegisterThumbnailRenderers();
}

void
FOdysseyAnimationEditorModule::ShutdownModule()
{
    // Unregister Commands
    UnregisterCommands();

    UnregisterSettings();

    // Unregister Assets Type Actions
    UnregisterAssetTypeActions();

    UnregisterLevelEditorLayoutExtensions();

    UnregisterDetailCustomization();

    UnregisterThumbnailRenderers();
}

void
FOdysseyAnimationEditorModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("asset-category.name", "ILIAD"));

    //Create Asset Types Actions
    mIliadTypeActions = MakeShareable(new FOdysseyAnimationAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::RegisterCommands()
{
    FOdysseyAnimationEditorCommands::Register();
}

void
FOdysseyAnimationEditorModule::UnregisterCommands()
{
    FOdysseyAnimationEditorCommands::Unregister();
}

void
FOdysseyAnimationEditorModule::RegisterDetailCustomizations()
{
    FOdysseyAnimationEditorFlipSystem::RegisterDetailCustomization();

    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout(UOdysseyAnimationEditorOutOfPegsTool::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyAnimationEditorOutOfPegsToolDetails::MakeInstance));
}

void
FOdysseyAnimationEditorModule::UnregisterDetailCustomization()
{
    FOdysseyAnimationEditorFlipSystem::UnregisterDetailCustomization();
}

void
FOdysseyAnimationEditorModule::RegisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyAnimationEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyAnimationEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
}

void
FOdysseyAnimationEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

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
FOdysseyAnimationEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditor" );
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyAnimationEditorUserSettings" );
}

void
FOdysseyAnimationEditorModule::RegisterThumbnailRenderers()
{
    UThumbnailManager::Get().RegisterCustomRenderer(UOdysseyAnimationCell::StaticClass(), UOdysseyAnimationCellThumbnailRenderer::StaticClass());
}

void
FOdysseyAnimationEditorModule::UnregisterThumbnailRenderers()
{
    //UThumbnailManager::Get().UnregisterCustomRenderer(UOdysseyAnimationCellImageRaster::StaticClass());
}

IMPLEMENT_MODULE( FOdysseyAnimationEditorModule, OdysseyAnimationEditor );

#undef LOCTEXT_NAMESPACE
