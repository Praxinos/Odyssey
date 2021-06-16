// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposTracksEditorModule.h"

#include "ISequencerModule.h"
#include "ISettingsModule.h"

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "CinematicBoardTrack/CinematicBoardTrackEditorCommands.h"
#include "SingleCameraCutTrack/SingleCameraCutTrackEditor.h"
#include "Settings/EposTracksSettings.h"

#define LOCTEXT_NAMESPACE "FEposTracksEditorModule"

//---

void
FEposTracksEditorModule::StartupModule()
{
    RegisterCommands();
    RegisterTrackEditors();
}

void
FEposTracksEditorModule::ShutdownModule()
{
    UnregisterTrackEditors();
    UnregisterCommands();
}

//---

void
FEposTracksEditorModule::RegisterCommands()
{
    FCinematicBoardTrackEditorCommands::Register();
}

void
FEposTracksEditorModule::UnregisterCommands()
{
    FCinematicBoardTrackEditorCommands::Unregister();
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

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposTracksEditorModule, EposTracksEditor )
