// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposEditorModule.h"

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
#include "Settings/EposEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Shot/ShotSequenceActions.h"
#include "Shot/ShotSequenceCustomization.h"
#include "Shot/ShotSequenceEditorCommands.h"
#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "FEposEditorModule"

EAssetTypeCategories::Type FEposEditorModule::mEposAssetCategory;

//---

FEposEditorModule::FEposEditorModule()
    : mSequencerSettings( nullptr )
{
}

//--- IModuleInterface interface

void
FEposEditorModule::StartupModule()
{
    RegisterCommands();
    RegisterAssetTools();
    RegisterMenuExtensions();
    RegisterSettings();
    RegisterSequenceCustomizations();
}

void
FEposEditorModule::ShutdownModule()
{
    UnregisterSequenceCustomizations();
    UnregisterSettings();
    UnregisterMenuExtensions();
    UnregisterAssetTools();
    UnregisterCommands();
}

//--- FGCObject interface

void
FEposEditorModule::AddReferencedObjects( FReferenceCollector& Collector )
{
    if( mSequencerSettings )
        Collector.AddReferencedObject( mSequencerSettings );
}

//---

void
FEposEditorModule::RegisterCommands()
{
    FBoardSequenceEditorCommands::Register();
    FShotSequenceEditorCommands::Register();
}

void
FEposEditorModule::UnregisterCommands()
{
    FShotSequenceEditorCommands::Unregister();
    FBoardSequenceEditorCommands::Unregister();
}

//---

//static
EAssetTypeCategories::Type
FEposEditorModule::GetAssetCategory()
{
    return mEposAssetCategory;
}

void
FEposEditorModule::RegisterAssetTools()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

    mEposAssetCategory = AssetTools.RegisterAdvancedAssetCategory( FName( TEXT( "EPOS" ) ), LOCTEXT( "EposAssetsCategory", "EPOS" ) );

    mBoardSequenceTypeActions = MakeShared<FBoardSequenceActions>( FEposEditorStyle::Get() );
    AssetTools.RegisterAssetTypeActions( mBoardSequenceTypeActions.ToSharedRef() );

    mShotSequenceTypeActions = MakeShared<FShotSequenceActions>( FEposEditorStyle::Get() );
    AssetTools.RegisterAssetTypeActions( mShotSequenceTypeActions.ToSharedRef() );
}

void
FEposEditorModule::UnregisterAssetTools()
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
FEposEditorModule::OnCreateNewAssetWithSettings( UClass* iClass )
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
FEposEditorModule::RegisterMenuExtensions()
{
    mCommandList = MakeShareable( new FUICommandList );
    mCommandList->MapAction( 
        FBoardSequenceEditorCommands::Get().NewBoardWithSettings,
        FExecuteAction::CreateStatic( &FEposEditorModule::OnCreateNewAssetWithSettings, UBoardSequence::StaticClass() )
    );
    mCommandList->MapAction( 
        FShotSequenceEditorCommands::Get().NewShotWithSettings,
        FExecuteAction::CreateStatic( &FEposEditorModule::OnCreateNewAssetWithSettings, UShotSequence::StaticClass() )
    );

    mCinematicsMenuExtender = MakeShareable( new FExtender );
    mCinematicsMenuExtender->AddMenuExtension( "LevelEditorNewCinematics", EExtensionHook::After, mCommandList, FMenuExtensionDelegate::CreateStatic( []( FMenuBuilder& MenuBuilder )
    {
        MenuBuilder.BeginSection( "CinematicsEpos", LOCTEXT( "CinematicsEpos", "Epos" ) );
        {
            MenuBuilder.AddMenuEntry( FBoardSequenceEditorCommands::Get().NewBoardWithSettings );
            MenuBuilder.AddMenuEntry( FShotSequenceEditorCommands::Get().NewShotWithSettings );
        }
        MenuBuilder.EndSection();
    } ) );

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
    LevelEditorModule.GetAllLevelEditorToolbarCinematicsMenuExtenders().Add( mCinematicsMenuExtender );
}

void
FEposEditorModule::UnregisterMenuExtensions()
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
FEposEditorModule::RegisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->RegisterSettings( "Project", "Plugins", "Epos",
                                        LOCTEXT( "EposSettingsName", "Epos" ),
                                        LOCTEXT( "EposSettingsDescription", "Configure the Epos Editor." ),
                                        GetMutableDefault<UEposEditorSettings>() );

    //---

    mSequencerSettings = USequencerSettingsContainer::GetOrCreate<USequencerSettings>( TEXT( "EposSequenceEditor" ) ); // May be initialized via Config directory.

    SettingsModule->RegisterSettings( "Editor", "ContentEditors", "EposSequenceEditor" /* Name used inside toolkit UniqueName */,
                                        LOCTEXT( "EposEditorSettingsName", "Epos Sequencer Editor" ),
                                        LOCTEXT( "EposEditorSettingsDescription", "Configure the look and feel of the Epos Sequencer Editor." ),
                                        mSequencerSettings );
}

void
FEposEditorModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->UnregisterSettings( "Project", "Plugins", "Epos" );
    SettingsModule->UnregisterSettings( "Editor", "ContentEditors", "EposSequenceEditor" );
}

//---

void
FEposEditorModule::RegisterSequenceCustomizations()
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
FEposEditorModule::UnregisterSequenceCustomizations()
{
    ISequencerModule& sequencerModule = FModuleManager::LoadModuleChecked<ISequencerModule>( "Sequencer" );

    sequencerModule.GetSequencerCustomizationManager()->UnregisterInstancedSequencerCustomization( UBoardSequence::StaticClass() );
    sequencerModule.GetSequencerCustomizationManager()->UnregisterInstancedSequencerCustomization( UShotSequence::StaticClass() );
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposEditorModule, EposEditor )
