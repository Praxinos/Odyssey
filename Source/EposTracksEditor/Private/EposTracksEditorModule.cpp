// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposTracksEditorModule.h"

#include "ISequencerModule.h"
#include "ISettingsModule.h"

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "SingleCameraCutTrack/SingleCameraCutTrackEditor.h"
#include "Settings/EposTracksEditorSettings.h"

#define LOCTEXT_NAMESPACE "FEposTracksEditorModule"

//---

void
FEposTracksEditorModule::StartupModule()
{
    RegisterSettings();
    RegisterTrackEditors();
}

void
FEposTracksEditorModule::ShutdownModule()
{
    UnregisterTrackEditors();
    UnregisterSettings();
}

//---

void
FEposTracksEditorModule::RegisterTrackEditors()
{
    if( GIsEditor )
    {
        ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );

        CinematicBoardTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FCinematicBoardTrackEditor::CreateTrackEditor ) );
        SingleCameraCutTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FSingleCameraCutTrackEditor::CreateTrackEditor ) );
    }

}
void
FEposTracksEditorModule::UnregisterTrackEditors()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>( "Sequencer" );

    SequencerModule.UnRegisterTrackEditor( CinematicBoardTrackCreateEditorHandle );
    SequencerModule.UnRegisterTrackEditor( SingleCameraCutTrackCreateEditorHandle );
}

void
FEposTracksEditorModule::RegisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->RegisterSettings( "Project", "Plugins", "EposTracks",
                                      LOCTEXT( "EposTracksSettingsName", "Epos Tracks" ),
                                      LOCTEXT( "EposTracksSettingsDescription", "Configure the Epos Tracks Editor." ),
                                      GetMutableDefault<UEposTracksEditorSettings>() );
}
void
FEposTracksEditorModule::UnregisterSettings()
{
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !SettingsModule )
        return;

    SettingsModule->UnregisterSettings( "Project", "Plugins", "EposTracks" );
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposTracksEditorModule, EposTracksEditor )
