// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "EposTracksEditorModule.h"

#include "ISequencerModule.h"

#include "BoardTrack/BoardTrackEditor.h"
#include "AudioTrack/AudioTrackEditor.h"
#include "LevelVisibilityTrack/LevelVisibilityTrackEditor.h"

#define LOCTEXT_NAMESPACE "FEposTracksEditorModule"

//---

void
FEposTracksEditorModule::StartupModule()
{
    if( GIsEditor )
    {
        ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );

        BoardTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FBoardTrackEditor::CreateTrackEditor ) );
        AudioTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FAudioTrackEditor::CreateTrackEditor ) );
        LevelVisibilityTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FLevelVisibilityTrackEditor::CreateTrackEditor ) );
    }
}

void
FEposTracksEditorModule::ShutdownModule()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().GetModuleChecked<ISequencerModule>( "Sequencer" );

    SequencerModule.UnRegisterTrackEditor( BoardTrackCreateEditorHandle );
    SequencerModule.UnRegisterTrackEditor( AudioTrackCreateEditorHandle );
    SequencerModule.UnRegisterTrackEditor( LevelVisibilityTrackCreateEditorHandle );
}

//---

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposTracksEditorModule, EposTracksEditor )
