// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceEditorModule.h"

#include "AssetToolsModule.h"
#include "Interfaces/IPluginManager.h" //PATCH
#include "ISequencerModule.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "SequencerSettings.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceActions.h"
#include "Board/BoardSequenceCustomization.h"
#include "EposSequenceEditorCommands.h"
#include "EposSequenceEditorNewStoryboardDialog.h"
#include "Render/EposSequencePipelineRenderer.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Settings/EposSequenceEditorSettingsCustomization.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceActions.h"
#include "Shot/ShotSequenceCustomization.h"
#include "StoryboardViewport/StoryboardViewportLayoutEntity.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "FEposEditorModule"

EAssetTypeCategories::Type FEposSequenceEditorModule::mEposAssetCategory;

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
    RegisterAssetTools();
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
    UnregisterAssetTools();
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

//static
EAssetTypeCategories::Type
FEposSequenceEditorModule::GetAssetCategory()
{
    return mEposAssetCategory;
}

void
FEposSequenceEditorModule::RegisterAssetTools()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

    mEposAssetCategory = AssetTools.RegisterAdvancedAssetCategory( FName( TEXT( "EPOS" ) ), LOCTEXT( "EposAssetsCategory", "EPOS" ) );

    mBoardSequenceTypeActions = MakeShared<FBoardSequenceActions>();
    AssetTools.RegisterAssetTypeActions( mBoardSequenceTypeActions.ToSharedRef() );

    mShotSequenceTypeActions = MakeShared<FShotSequenceActions>();
    AssetTools.RegisterAssetTypeActions( mShotSequenceTypeActions.ToSharedRef() );
}

void
FEposSequenceEditorModule::UnregisterAssetTools()
{
    FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>( "AssetTools" );
    if( !AssetToolsModule )
        return;

    IAssetTools& AssetTools = AssetToolsModule->Get();
    AssetTools.UnregisterAssetTypeActions( mBoardSequenceTypeActions.ToSharedRef() );
    AssetTools.UnregisterAssetTypeActions( mShotSequenceTypeActions.ToSharedRef() );
}

//static
void
FEposSequenceEditorModule::OnCreateNewAssetWithSettings( UClass* iClass )
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );
    NewStoryboardDialog::OpenDialog( LevelEditorModule.GetLevelEditorTabManager().ToSharedRef() );
}

void
FEposSequenceEditorModule::RegisterMenuExtensions()
{
    mCommandList = MakeShareable( new FUICommandList );
    mCommandList->MapAction(
        FEposSequenceEditorCommands::Get().NewStoryboardWithSettings,
        FExecuteAction::CreateStatic( &FEposSequenceEditorModule::OnCreateNewAssetWithSettings, UBoardSequence::StaticClass() )
    );

    mCinematicsMenuExtender = MakeShareable( new FExtender );
    mCinematicsMenuExtender->AddMenuExtension( "LevelEditorNewCinematics", EExtensionHook::After, mCommandList, FMenuExtensionDelegate::CreateStatic( []( FMenuBuilder& MenuBuilder )
    {
        MenuBuilder.BeginSection( "CinematicsEpos", LOCTEXT( "CinematicsEpos", "Epos" ) );
        {
            MenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().NewStoryboardWithSettings );
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
    TSharedPtr<IPlugin> epos_plugin = IPluginManager::Get().FindPlugin( "Epos" );
    FString PluginConfigDir = epos_plugin->GetBaseDir() / TEXT( "Config/" );
    mSequencerSettings->LoadConfig( USequencerSettings::StaticClass(), *FPaths::Combine( PluginConfigDir, TEXT( "EditorPerProjectUserSettings.ini" ) ) );
    //~

    SettingsModule->RegisterSettings( "Editor", "ContentEditors", "EposSequencerEditor" /* Name used inside toolkit UniqueName */,
                                        LOCTEXT( "EposSequencerEditorSettingsName", "Epos Sequencer Editor" ),
                                        LOCTEXT( "EposSequencerEditorSettingsDescription", "Configure the look and feel of the Epos Sequencer Editor." ),
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

    PropertyModule.NotifyCustomizationModuleChanged();
}

void
FEposSequenceEditorModule::UnregisterPropertyCustomizations()
{
    if( FModuleManager::Get().IsModuleLoaded( "PropertyEditor" ) )
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FInfoBarSettings::StaticStruct()->GetFName() );

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

//---

void
FEposSequenceEditorModule::RegisterMovieRenderer()
{
    ISequencerModule& SequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );

    mMovieRendererDelegate = SequencerModule.RegisterMovieRenderer( TUniquePtr<IMovieRendererInterface>( new FEposSequencePipelineRenderer ) );
}

void
FEposSequenceEditorModule::UnregisterMovieRenderer()
{
    ISequencerModule* SequencerModule = FModuleManager::GetModulePtr<ISequencerModule>( "Sequencer" );
    if( SequencerModule )
    {
        SequencerModule->UnregisterMovieRenderer( mMovieRendererDelegate );
    }
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposSequenceEditorModule, EposSequenceEditor )
