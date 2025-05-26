// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposTracksEditorModule.h"

#include "ISequencerModule.h"
#include "ISettingsModule.h"

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "EposTracksEditorCommands.h"
#include "SingleCameraCutTrack/SingleCameraCutTrackEditor.h"
#include "NoteTrack/NoteTrackEditor.h"
#include "Settings/EposTracksSettings.h"
#include "Styles/EposTracksEditorStyle.h"

#define LOCTEXT_NAMESPACE "FEposTracksEditorModule"

//---

void
FEposTracksEditorModule::StartupModule()
{
    FEposTracksEditorStyle::Register(); // Must be done before Commands(), as it uses style
    RegisterCommands();
    RegisterTrackEditors();
}

void
FEposTracksEditorModule::ShutdownModule()
{
    UnregisterTrackEditors();
    UnregisterCommands();
    FEposTracksEditorStyle::Unregister();
}

//---

void
FEposTracksEditorModule::RegisterCommands()
{
    FEposTracksEditorCommands::Register();
}

void
FEposTracksEditorModule::UnregisterCommands()
{
    FEposTracksEditorCommands::Unregister();
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
        NoteTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FNoteTrackEditor::CreateTrackEditor ) );
    }

}
void
FEposTracksEditorModule::UnregisterTrackEditors()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>( "Sequencer" );

    SequencerModule.UnRegisterTrackEditor( CinematicBoardTrackCreateEditorHandle );
    SequencerModule.UnRegisterTrackEditor( SingleCameraCutTrackCreateEditorHandle );
    SequencerModule.UnRegisterTrackEditor( NoteTrackCreateEditorHandle );
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposTracksEditorModule, EposTracksEditor )
