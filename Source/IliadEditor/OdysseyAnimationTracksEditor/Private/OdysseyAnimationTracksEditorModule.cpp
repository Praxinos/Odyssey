// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTracksEditorModule.h"

#include "ILevelSequenceModule.h"
#include "ISequencerModule.h"
#include "OdysseyAnimationTimelineTrackEditor.h"

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
}

void
FOdysseyAnimationTracksEditorModule::UnregisterSequencerTracks()
{
    ISequencerModule& SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>( "Sequencer" );
    SequencerModule.UnRegisterTrackEditor( mAnimationTrackCreateEditorHandle );

    ILevelSequenceModule& LevelSequenceModule = FModuleManager::LoadModuleChecked<ILevelSequenceModule>("LevelSequence");
    LevelSequenceModule.OnNewActorTrackAdded().Remove( mOnNewActorTrackAddedHandle );
}


IMPLEMENT_MODULE( FOdysseyAnimationTracksEditorModule, OdysseyAnimationTracksEditorModule );

#undef LOCTEXT_NAMESPACE
