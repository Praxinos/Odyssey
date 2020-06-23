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
#include "Settings/EposEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposEditorStyle.h"

#define LOCTEXT_NAMESPACE "FEposEditorModule"

//---

FEposEditorModule::FEposEditorModule()
    : mSettings( nullptr )
{
}

//--- IModuleInterface interface

void
FEposEditorModule::StartupModule()
{
    RegisterAssetTools();
    RegisterSettings();
}

void
FEposEditorModule::ShutdownModule()
{
    UnregisterSettings();
    UnregisterAssetTools();
}

//--- FGCObject interface

void
FEposEditorModule::AddReferencedObjects( FReferenceCollector& Collector )
{
    if( mSettings )
        Collector.AddReferencedObject( mSettings );
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

    mSettings = USequencerSettingsContainer::GetOrCreate<USequencerSettings>( TEXT( "EposEditor" ) ); // Don't know how to use/manage them

    SettingsModule->RegisterSettings( "Editor", "ContentEditors", "EposEditor",
                                        LOCTEXT( "EposEditorSettingsName", "Epos Editor" ),
                                        LOCTEXT( "EposEditorSettingsDescription", "Configure the look and feel of the Epos Editor." ),
                                        mSettings );
}

void
FEposEditorModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->UnregisterSettings( "Project", "Plugins", "Epos" );
    SettingsModule->UnregisterSettings( "Editor", "ContentEditors", "EposEditor" );
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposEditorModule, EposEditor )
