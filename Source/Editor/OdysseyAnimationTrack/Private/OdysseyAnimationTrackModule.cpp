// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTrackModule.h"

#include "ILevelSequenceModule.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

/*-----------------------------------------------------------------------------
   FOdysseyAnimationTrackModule
-----------------------------------------------------------------------------*/

void
FOdysseyAnimationTrackModule::StartupModule()
{
    RegisterSequencerTracks();
}

void
FOdysseyAnimationTrackModule::ShutdownModule()
{
    UnregisterSequencerTracks();
}

void
FOdysseyAnimationTrackModule::RegisterSequencerTracks()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    mAnimationTrackCreateEditorHandle = SequencerModule.RegisterTrackEditor( FOnCreateTrackEditor::CreateStatic( &FOdysseyAnimationTrackEditor::CreateTrackEditor ) );

    ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
    mOnNewActorTrackAddedHandle = LevelSequenceModule.OnNewActorTrackAdded().AddStatic( FOdysseyAnimationTrackEditor::OnNewActorTrackAdded );
}

void
FOdysseyAnimationTrackModule::UnregisterSequencerTracks()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    SequencerModule.UnRegisterTrackEditor( mAnimationTrackCreateEditorHandle );

    ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
    LevelSequenceModule.OnNewActorTrackAdded().Remove( mOnNewActorTrackAddedHandle );
}


IMPLEMENT_MODULE( FOdysseyAnimationTrackModule, OdysseyAnimationEditor );

#undef LOCTEXT_NAMESPACE
