// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationTracksEditorModule.h"

#include "EposSequenceModule.h"
#include "ILevelSequenceModule.h"
#include "ISequencerModule.h"
#include "OdysseyAnimationCutChannel.h"
#include "OdysseyAnimationTimelineTrackEditor.h"
#include "SequencerChannelInterface.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

/*-----------------------------------------------------------------------------
   FOdysseyAnimationTracksEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyAnimationTracksEditorModule::StartupModule()
{
    RegisterSequencerTracks();
}

void
FOdysseyAnimationTracksEditorModule::ShutdownModule()
{
    UnregisterSequencerTracks();
}

void
FOdysseyAnimationTracksEditorModule::RegisterSequencerTracks()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    mAnimationTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FOdysseyAnimationTimelineTrackEditor::CreateTrackEditor ) );

    ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
    mOnNewActorTrackAddedHandle = LevelSequenceModule.OnNewActorTrackAdded().AddStatic( FOdysseyAnimationTimelineTrackEditor::OnNewActorTrackAdded );
    FEposSequenceModule& eposSequenceModule = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );
    mOnNewActorTrackAddedOnEposHandle = eposSequenceModule.OnNewActorTrackAdded().AddStatic( FOdysseyAnimationTimelineTrackEditor::OnNewActorTrackAdded );

    SequencerModule.RegisterChannelInterface<FOdysseyAnimationCutChannel>();
}

void
FOdysseyAnimationTracksEditorModule::UnregisterSequencerTracks()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    SequencerModule.UnRegisterTrackEditor( mAnimationTrackCreateEditorHandle );

    ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
    LevelSequenceModule.OnNewActorTrackAdded().Remove( mOnNewActorTrackAddedHandle );
    FEposSequenceModule& eposSequenceModule = FModuleManager::LoadModuleChecked<FEposSequenceModule>( "EposSequence" );
    eposSequenceModule.OnNewActorTrackAdded().Remove( mOnNewActorTrackAddedOnEposHandle );
}


IMPLEMENT_MODULE( FOdysseyAnimationTracksEditorModule, OdysseyAnimationTracksEditor );

#undef LOCTEXT_NAMESPACE
