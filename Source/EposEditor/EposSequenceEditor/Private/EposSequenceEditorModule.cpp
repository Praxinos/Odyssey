// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceEditorModule.h"

#include "AssetToolsModule.h"
#include "Interfaces/IPluginManager.h" //PATCH
#include "ISequencerModule.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Sequencer/EditModeAnimationUtil.h"
#include "SequencerSettings.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceCustomization.h"
#include "EposSequenceEditorCommands.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "Export/ImageSequence/ExportImageSequenceSettingsCustomization.h"
#include "Export/ExportSequencerRenderer.h"
#include "Import/ImportImageSequenceSettings.h"
#include "Import/ImportImageSequenceSettingsCustomization.h"
#include "Render/EposSequencePipelineRenderer.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/EposSequenceEditorSettingsCustomization.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceCustomization.h"
#include "StoryboardCreationDialog/NewStoryboardDialog.h"
#include "StoryboardViewport/StoryboardViewportLayoutEntity.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "FEposEditorModule"

//---

FEposSequenceEditorModule::FEposSequenceEditorModule()
    : mSequencerSettings( nullptr )
{
}

//--- IModuleInterface interface

void
FEposSequenceEditorModule::StartupModule()
{
    FEposSequenceEditorStyle::Register(); // Must be done before Commands(), as it uses style
    RegisterCommands();
    RegisterMenuExtensions();
    RegisterLevelEditorExtensions();
    RegisterSettings();
    RegisterSequenceCustomizations();
    RegisterPropertyCustomizations();
    RegisterMovieRenderer();
}

void
FEposSequenceEditorModule::ShutdownModule()
{
    UnregisterMovieRenderer();
    UnregisterPropertyCustomizations();
    UnregisterSequenceCustomizations();
    UnregisterSettings();
    UnregisterMenuExtensions();
    UnregisterLevelEditorExtensions();
    UnregisterCommands();
    FEposSequenceEditorStyle::Unregister();
}

//--- FGCObject interface

void
FEposSequenceEditorModule::AddReferencedObjects( FReferenceCollector& Collector )
{
    if( mSequencerSettings )
        Collector.AddReferencedObject( mSequencerSettings );
}

FString
FEposSequenceEditorModule::GetReferencerName() const //override
{
    return "FEposSequenceEditorModule";
}

//---

void
FEposSequenceEditorModule::RegisterCommands()
{
    FEposSequenceEditorCommands::Register();
}

void
FEposSequenceEditorModule::UnregisterCommands()
{
    FEposSequenceEditorCommands::Unregister();
}

//---

void
FEposSequenceEditorModule::RegisterMenuExtensions()
{
    mCommandList = MakeShareable( new FUICommandList );
    mCommandList->MapAction(
        FEposSequenceEditorCommands::Get().NewStoryboardWithSettings,
        FExecuteAction::CreateLambda( []()
                                      {
                                          FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
                                          NewStoryboardDialog::OpenCreationDialog( LevelEditorModule.GetLevelEditorTabManager().ToSharedRef() );
                                      } )
    );
    mCommandList->MapAction(
        FEposSequenceEditorCommands::Get().NewStoryboardImportImageSequence,
        FExecuteAction::CreateLambda( []()
                                      {
                                          FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
                                          NewStoryboardDialog::OpenImportImageSequenceDialog( LevelEditorModule.GetLevelEditorTabManager().ToSharedRef() );
                                      } )
    );

    mCinematicsMenuExtender = MakeShareable( new FExtender );
    mCinematicsMenuExtender->AddMenuExtension( "LevelEditorNewCinematics", EExtensionHook::After, mCommandList, FMenuExtensionDelegate::CreateLambda( []( FMenuBuilder& MenuBuilder )
    {
        MenuBuilder.BeginSection( "CinematicsOdyssey", LOCTEXT( "CinematicsOdyssey", "Odyssey" ) );
        {
            MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().NewStoryboardWithSettings );
            MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().NewStoryboardImportImageSequence );
        }
        MenuBuilder.EndSection();
    } ) );

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
    LevelEditorModule.GetAllLevelEditorToolbarCinematicsMenuExtenders().Add( mCinematicsMenuExtender );
}

void
FEposSequenceEditorModule::UnregisterMenuExtensions()
{
    FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>( "LevelEditor" );
    if( !LevelEditorModule )
        return;

    LevelEditorModule->GetAllLevelEditorToolbarCinematicsMenuExtenders().Remove( mCinematicsMenuExtender );
    mCinematicsMenuExtender = nullptr;
    mCommandList = nullptr;
}

//---

void
FEposSequenceEditorModule::RegisterLevelEditorExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );

    FViewportTypeDefinition StoryboardViewportType = FViewportTypeDefinition::FromType<FStoryboardViewportLayoutEntity>( FEposSequenceEditorCommands::Get().ToggleStoryboardViewportCommand );
    LevelEditorModule.RegisterViewportType( "Storyboard", StoryboardViewportType );

    //---

    UE::AnimationEditMode::FCustomMovieSceneRegistry& registry = UE::AnimationEditMode::FCustomMovieSceneRegistry::Get();
    registry.RegisterSequence<UShotSequence>();
    registry.RegisterSequence<UBoardSequence>();
}

void
FEposSequenceEditorModule::UnregisterLevelEditorExtensions()
{
    FLevelEditorModule* LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>( "LevelEditor" );
    if( !LevelEditorModule )
        return;

    LevelEditorModule->UnregisterViewportType( "Storyboard" );
}

//---

void
FEposSequenceEditorModule::RegisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    mSequencerSettings = USequencerSettingsContainer::GetOrCreate<USequencerSettings>( TEXT( "EposSequencerEditor" ) ); // May be initialized via Config directory.

    //PATCH: Should be done AUTOMATICALLY via EditorPerProjectUserSettings.ini config file, but doesn't work in 5.0
    TSharedPtr<IPlugin> epos_plugin = IPluginManager::Get().FindPlugin( "Odyssey" );
    FString PluginConfigDir = epos_plugin->GetBaseDir() / TEXT( "Config/" );
    // 5.1: With the warning of EditorPerProjectUserSettings.ini file which shouldn't be overrided in the plugin
    //mSequencerSettings->LoadConfig( USequencerSettings::StaticClass(), *FPaths::Combine( PluginConfigDir, TEXT( "EditorPerProjectUserSettings.ini" ) ) );
    // Move its content to BaseOdyssey.ini
    // This line will override the user config values

    FString configPath = FConfigCacheIni::NormalizeConfigIniPath( FPaths::Combine( PluginConfigDir, TEXT( "BaseOdyssey.ini" ) ) );
    mSequencerSettings->LoadConfig( USequencerSettings::StaticClass(), *configPath );
    // This line is to load again the user config values
    // Otherwise, the values will always be the ones inside BaseOdyssey.ini (like ZeroPaddedFrame which will always be 4)
    mSequencerSettings->LoadConfig();
    //~

    SettingsModule->RegisterSettings( "Editor", "ContentEditors", "EposSequencerEditor" /* Name used inside toolkit UniqueName */,
                                        LOCTEXT( "EposSequencerEditorSettingsName", "Odyssey - Storyboard Sequencer Editor" ),
                                        LOCTEXT( "EposSequencerEditorSettingsDescription", "Configure the look and feel of the Odyssey Storyboard Sequencer Editor." ),
                                        mSequencerSettings );
}

void
FEposSequenceEditorModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->UnregisterSettings( "Editor", "ContentEditors", "EposSequencerEditor" );
}

//---

void
FEposSequenceEditorModule::RegisterSequenceCustomizations()
{
    ISequencerModule& sequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );

    sequencerModule.GetSequencerCustomizationManager()->RegisterInstancedSequencerCustomization( UBoardSequence::StaticClass(),
        FOnGetSequencerCustomizationInstance::CreateLambda( []()
        {
            return new FBoardSequenceCustomization();
        } ) );
    sequencerModule.GetSequencerCustomizationManager()->RegisterInstancedSequencerCustomization( UShotSequence::StaticClass(),
        FOnGetSequencerCustomizationInstance::CreateLambda( []()
        {
            return new FShotSequenceCustomization();
        } ) );
}

void
FEposSequenceEditorModule::UnregisterSequenceCustomizations()
{
    ISequencerModule& sequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );

    sequencerModule.GetSequencerCustomizationManager()->UnregisterInstancedSequencerCustomization( UBoardSequence::StaticClass() );
    sequencerModule.GetSequencerCustomizationManager()->UnregisterInstancedSequencerCustomization( UShotSequence::StaticClass() );
}

void
FEposSequenceEditorModule::RegisterPropertyCustomizations()
{
    // import the PropertyEditor module...
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

    // to register our custom property
    PropertyModule.RegisterCustomPropertyTypeLayout(
        // This is the name of the Struct
        // this tells the property editor which is the struct property our customization will applied on.
        FInfoBarSettings::StaticStruct()->GetFName(),
        // this is where our MakeInstance() method is usefull
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FInfoBarCustomization::MakeInstance ) );

    PropertyModule.RegisterCustomPropertyTypeLayout(
        FImportImageSequenceOptions::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FImportImageSequenceOptionsCustomization::MakeInstance ) );

    PropertyModule.RegisterCustomPropertyTypeLayout(
        FExportImageSequenceOptions::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FExportImageSequenceOptionsCustomization::MakeInstance ) );

    PropertyModule.NotifyCustomizationModuleChanged();
}

void
FEposSequenceEditorModule::UnregisterPropertyCustomizations()
{
    if( FModuleManager::Get().IsModuleLoaded( "PropertyEditor" ) )
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FInfoBarSettings::StaticStruct()->GetFName() );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FImportImageSequenceOptions::StaticStruct()->GetFName() );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FExportImageSequenceOptions::StaticStruct()->GetFName() );

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

//---

void
FEposSequenceEditorModule::RegisterMovieRenderer()
{
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );

    mMovieRendererDelegate = SequencerModule.RegisterMovieRenderer( TUniquePtr<IMovieRendererInterface>( new FEposSequencePipelineRenderer ) );
    mExportSequencerRendererDelegate = SequencerModule.RegisterMovieRenderer( TUniquePtr<IMovieRendererInterface>( new FExportSequencerRenderer ) );
}

void
FEposSequenceEditorModule::UnregisterMovieRenderer()
{
    ISequencerModule* SequencerModule = FModuleManager::GetModulePtr<ISequencerModule>( "Sequencer" );
    if( SequencerModule )
    {
        SequencerModule->UnregisterMovieRenderer( mMovieRendererDelegate );
        SequencerModule->UnregisterMovieRenderer( mExportSequencerRendererDelegate );
    }
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposSequenceEditorModule, EposSequenceEditor )
