// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposTracksEditorModule.h"

#include "ISequencerModule.h"

#include "BoardTrack/BoardTrackEditor.h"

#define LOCTEXT_NAMESPACE "FEposTracksEditorModule"

//---

void
FEposTracksEditorModule::StartupModule()
{
    if( GIsEditor )
    {
        ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );

        BoardTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FBoardTrackEditor::CreateTrackEditor ) );
    }
}

void
FEposTracksEditorModule::ShutdownModule()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>( "Sequencer" );

    SequencerModule.UnRegisterTrackEditor( BoardTrackCreateEditorHandle );
}

//---

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEposTracksEditorModule, EposTracksEditor)
