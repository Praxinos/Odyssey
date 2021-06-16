// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposSequenceEditorModule.h"

#include "AssetToolsModule.h"
#include "ISequencerModule.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "SequencerSettings.h"

#include "Board/BoardSequence.h"
#include "Board/BoardSequenceActions.h"
#include "Board/BoardSequenceCustomization.h"
#include "Board/BoardSequenceEditorCommands.h"
#include "EposSequenceEditorCommands.h"
#include "Settings/EposSequenceEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceActions.h"
#include "Shot/ShotSequenceCustomization.h"
#include "Shot/ShotSequenceEditorCommands.h"
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
    RegisterCommands();
    RegisterAssetTools();
    RegisterMenuExtensions();
    RegisterLevelEditorExtensions();
    RegisterSettings();
    RegisterSequenceCustomizations();
}

void
FEposSequenceEditorModule::ShutdownModule()
{
    UnregisterSequenceCustomizations();
    UnregisterSettings();
    UnregisterMenuExtensions();
    UnregisterLevelEditorExtensions();
    UnregisterAssetTools();
    UnregisterCommands();
}

//--- FGCObject interface

void
FEposSequenceEditorModule::AddReferencedObjects( FReferenceCollector& Collector )
{
    if( mSequencerSettings )
        Collector.AddReferencedObject( mSequencerSettings );
}

//---

void
FEposSequenceEditorModule::RegisterCommands()
{
    FEposSequenceEditorCommands::Register();
    FBoardSequenceEditorCommands::Register();
    FShotSequenceEditorCommands::Register();
}

void
FEposSequenceEditorModule::UnregisterCommands()
{
    FShotSequenceEditorCommands::Unregister();
    FBoardSequenceEditorCommands::Unregister();
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

    mBoardSequenceTypeActions = MakeShared<FBoardSequenceActions>( FEposSequenceEditorStyle::Get() );
    AssetTools.RegisterAssetTypeActions( mBoardSequenceTypeActions.ToSharedRef() );

    mShotSequenceTypeActions = MakeShared<FShotSequenceActions>( FEposSequenceEditorStyle::Get() );
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
    // Create a new level sequence
    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

    UObject* NewAsset = nullptr;

    // Attempt to create a new asset
    for( auto factory : AssetTools.GetNewAssetFactories() )
    {
        if( factory->CanCreateNew() && factory->ImportPriority >= 0 && factory->SupportedClass == iClass )
        {
            NewAsset = AssetTools.CreateAssetWithDialog( iClass, factory );
            break;
        }
    }

    if( !NewAsset )
        return;

    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset( NewAsset );
}

void
FEposSequenceEditorModule::RegisterMenuExtensions()
{
    mCommandList = MakeShareable( new FUICommandList );
    mCommandList->MapAction(
        FBoardSequenceEditorCommands::Get().NewStoryboardWithSettings,
        FExecuteAction::CreateStatic( &FEposSequenceEditorModule::OnCreateNewAssetWithSettings, UBoardSequence::StaticClass() )
    );

    mCinematicsMenuExtender = MakeShareable( new FExtender );
    mCinematicsMenuExtender->AddMenuExtension( "LevelEditorNewCinematics", EExtensionHook::After, mCommandList, FMenuExtensionDelegate::CreateStatic( []( FMenuBuilder& MenuBuilder )
    {
        MenuBuilder.BeginSection( "CinematicsEpos", LOCTEXT( "CinematicsEpos", "Epos" ) );
        {
            MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().NewStoryboardWithSettings );
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

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposSequenceEditorModule, EposSequenceEditor )
