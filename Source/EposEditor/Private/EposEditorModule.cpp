// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposEditorModule.h"

#include "AssetToolsModule.h"
#include "ISequencerModule.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "SequencerSettings.h"

#include "AssetTools/BoardSequenceActions.h"
#include "AssetTools/ShotSequenceActions.h"
#include "Board/BoardSequence.h"
#include "BoardSequenceEditorCommands.h"
#include "Customization/BoardSequenceCustomization.h"
#include "Customization/ShotSequenceCustomization.h"
#include "Settings/EposEditorSettings.h"
#include "Settings/EposSequencerSettings.h"
#include "Shot/ShotSequence.h"
#include "ShotSequenceEditorCommands.h"
#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "FEposEditorModule"

//---

FEposEditorModule::FEposEditorModule()
    : mSettingsBoard( nullptr )
    , mSettingsShot( nullptr )
{
}

//--- IModuleInterface interface

void
FEposEditorModule::StartupModule()
{
    RegisterCommands();
    RegisterAssetTools();
    RegisterSettings();
    RegisterSequenceCustomizations();
}

void
FEposEditorModule::ShutdownModule()
{
    UnregisterSequenceCustomizations();
    UnregisterSettings();
    UnregisterAssetTools();
    UnregisterCommands();
}

//--- FGCObject interface

void
FEposEditorModule::AddReferencedObjects( FReferenceCollector& Collector )
{
    if( mSettingsBoard )
        Collector.AddReferencedObject( mSettingsBoard );
    if( mSettingsShot )
        Collector.AddReferencedObject( mSettingsShot );
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

void
FEposEditorModule::RegisterAssetTools()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();

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

    mSettingsBoard = USequencerSettingsContainer::GetOrCreate<USequencerSettings>( TEXT( "ShotSequenceEditor" ) ); // Don't know how to use/manage them -> Inside Config directory.

    SettingsModule->RegisterSettings( "Editor", "ContentEditors", "ShotSequenceEditor",
                                        LOCTEXT( "EposEditorSettingsShotName", "Epos Editor (ShotSequence)" ),
                                        LOCTEXT( "EposEditorSettingsShotDescription", "Configure the look and feel of the Epos Editor (ShotSequence)." ),
                                        mSettingsBoard );

    mSettingsShot = USequencerSettingsContainer::GetOrCreate<UEposSequencerSettings>( TEXT( "BoardSequenceEditor" ) ); // Don't know how to use/manage them -> Inside Config directory.

    SettingsModule->RegisterSettings( "Editor", "ContentEditors", "BoardSequenceEditor",
                                        LOCTEXT( "EposEditorSettingsBoardName", "Epos Editor (BoardSequence)" ),
                                        LOCTEXT( "EposEditorSettingsBoardDescription", "Configure the look and feel of the Epos Editor (BoardSequence)." ),
                                        mSettingsShot );
}

void
FEposEditorModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->UnregisterSettings( "Project", "Plugins", "Epos" );
    SettingsModule->UnregisterSettings( "Editor", "ContentEditors", "ShotSequenceEditor" );
    SettingsModule->UnregisterSettings( "Editor", "ContentEditors", "BoardSequenceEditor" );
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
