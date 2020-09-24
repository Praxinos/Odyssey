// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposTracksEditorModule.h"

#include "ISequencerModule.h"

#include "CinematicBoardTrack/CinematicBoardTrackEditor.h"
#include "SingleCameraCutTrack/SingleCameraCutTrackEditor.h"

#define LOCTEXT_NAMESPACE "FEposTracksEditorModule"

//---

void
FEposTracksEditorModule::StartupModule()
{
    if( GIsEditor )
    {
        ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );

        CinematicBoardTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FCinematicBoardTrackEditor::CreateTrackEditor ) );
        SingleCameraCutTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FSingleCameraCutTrackEditor::CreateTrackEditor ) );
    }
}

void
FEposTracksEditorModule::ShutdownModule()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>( "Sequencer" );

    SequencerModule.UnRegisterTrackEditor( CinematicBoardTrackCreateEditorHandle );
    SequencerModule.UnRegisterTrackEditor( SingleCameraCutTrackCreateEditorHandle );
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposTracksEditorModule, EposTracksEditor )
