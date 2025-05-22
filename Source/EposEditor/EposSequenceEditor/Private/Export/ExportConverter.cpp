// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportConverter.h"

#include "AssetToolsModule.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "CommonFrameRates.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "ISequencer.h"
#include "MovieSceneSequenceVisitor.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "NamingConvention.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyLayerStack.h"
#include "Shot/ShotSequence.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ExportConverter"

//---

FExportConverter::FExportConverter( TWeakPtr<ISequencer> iSequencer, FMovieSceneSequenceIDRef iSequenceId, const FExportMarkSettings* iMarkSettings, FExportStruct* oStruct )
    : mSequencer( iSequencer )
    , mSequenceId( iSequenceId )
    , mMarkSettings( iMarkSettings )
    , mStruct( oStruct )
{
    mStruct->mSequencer = mSequencer;

    Convert();
}

void
FExportConverter::ProcessSequencerMarks( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const
{
    TArray<FMovieSceneMarkedFrame> marks = iShotSequence.GetMovieScene()->GetMarkedFrames();
    for( auto mark : marks )
    {
        FMovieSceneInverseSequenceTransform localToRootTransform = iRootToSequenceTransform.Inverse();
        TOptional<FFrameTime> time_in_root = localToRootTransform.TryTransformTime( mark.FrameNumber );
        if( !time_in_root )
            continue;

        FFrameNumber frame_in_root = time_in_root->GetFrame();

        FExportPanel* existing_panel = ioPanels.FindByPredicate( [frame_in_root]( const FExportPanel& iElement )
                                                                {
                                                                    return iElement.GlobalFrame == frame_in_root;
                                                                } );
        if( existing_panel )
        {
            existing_panel->mSourceMark = { mark };
        }
        else
        {
            FExportPanel panel;
            panel.GlobalFrame = frame_in_root;
            panel.mSequence = &iShotSequence;
            panel.mSequenceId = iSequenceId;
            panel.mSourceMark = { mark };

            ioPanels.Add( panel );
        }
    }
}

void
FExportConverter::ProcessAnimationCuts( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    TArray<AOdysseyAnimationActor*> animation_actors;
    TArray<FGuid> animation_bindings;
    /*int32 num_animations =*/ ShotSequenceHelpers::GetAllAnimations( *sequencer, &iShotSequence, iSequenceId, EGetAnimation::kAll, &animation_actors, &animation_bindings );

    for( FGuid animation_binding : animation_bindings )
    {
        AOdysseyAnimationActor* animation_actor = animation_actors[animation_bindings.IndexOfByKey( animation_binding )];
        check( animation_actor );
        ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( *sequencer, &iShotSequence, iSequenceId, animation_binding );
        check( animation_actors.Contains( result.mAnimationActor.Get() ) );

        for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> section : result.mSections )
        {
            const FOdysseyAnimationCutChannel& channel = section->GetAnimationCutChannel();
            TMovieSceneChannelData<const FOdysseyAnimationCutValue> data = channel.GetData();
            TArrayView<const FFrameNumber> times = data.GetTimes();
            TArrayView<const FOdysseyAnimationCutValue> values = data.GetValues();
            for( int i = 0; i < times.Num(); i++ )
            {
                const FFrameNumber& frame_in_sequence = times[i];
                const FOdysseyAnimationCutValue& value = values[i];

                FMovieSceneInverseSequenceTransform localToRootTransform = iRootToSequenceTransform.Inverse();
                TOptional<FFrameTime> time_in_root = localToRootTransform.TryTransformTime( frame_in_sequence );
                if( !time_in_root )
                    continue;

                FFrameNumber frame_in_root = time_in_root->GetFrame();

                FExportPanel* existing_panel = ioPanels.FindByPredicate( [frame_in_root]( const FExportPanel& iElement )
                                                                         {
                                                                             return iElement.GlobalFrame == frame_in_root;
                                                                         } );
                if( existing_panel )
                {
                    check( existing_panel->mSequence == &iShotSequence );

                    if( !existing_panel->mSourceAnimationCut.IsSet() )
                        existing_panel->mSourceAnimationCut = FExportPanelSourceAnimationCut();

                    existing_panel->mSourceAnimationCut.GetValue().mAnimationCuts.Add( { frame_in_sequence, animation_binding } );
                }
                else
                {
                    FExportPanel panel;
                    panel.GlobalFrame = frame_in_root;
                    panel.mSequence = &iShotSequence;
                    panel.mSequenceId = iSequenceId;
                    FExportPanelSourceAnimationCut source_animationcut;
                    source_animationcut.mAnimationCuts.Add( { frame_in_sequence, animation_binding } );
                    panel.mSourceAnimationCut = source_animationcut;

                    ioPanels.Add( panel );
                }
            }
        }
    }
}

void
FExportConverter::ProcessFirstShotFrame( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const
{
    TRange<FFrameNumber> playback_range = iShotSequence.GetMovieScene()->GetPlaybackRange();

    FMovieSceneInverseSequenceTransform localToRootTransform = iRootToSequenceTransform.Inverse();
    TOptional<FFrameTime> time_in_root = localToRootTransform.TryTransformTime( playback_range.GetLowerBoundValue() );
    if( !time_in_root )
        return;

    FFrameNumber frame_in_root = time_in_root->GetFrame();

    FExportPanel* existing_panel = ioPanels.FindByPredicate( [frame_in_root]( const FExportPanel& iElement )
                                                            {
                                                                return iElement.GlobalFrame == frame_in_root;
                                                            } );
    if( existing_panel )
    {
        check( existing_panel->mSequence == &iShotSequence );

        // If there is already in the panel list, nothing to do as we store only the frame for this case
    }
    else
    {
        FExportPanel panel;
        panel.GlobalFrame = frame_in_root;
        panel.mSequence = &iShotSequence;
        panel.mSequenceId = iSequenceId;

        ioPanels.Add( panel );
    }
}
void
FExportConverter::Convert()
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    //---

    struct FSequenceShotVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitSubSequence( UMovieSceneSequence* iSequence, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UShotSequence* shot_sequence = Cast<UShotSequence>( iSequence );
            if( !shot_sequence )
                return;

            UE::MovieScene::FSubSequencePath subsequencepath( iLocalSpace.SequenceID, mSequencer->GetSharedPlaybackState() );

            if( !subsequencepath.Contains( mSequenceId ) )
                return;

            //---

            if( mMarkSettings->Marks )
                mConverter->ProcessSequencerMarks( *shot_sequence, iLocalSpace.SequenceID, iLocalSpace.RootToSequenceTransform, mPanels );

            if( mMarkSettings->AnimationCuts )
                mConverter->ProcessAnimationCuts( *shot_sequence, iLocalSpace.SequenceID, iLocalSpace.RootToSequenceTransform, mPanels );

            if( mMarkSettings->FirstFrameOfShot )
                mConverter->ProcessFirstShotFrame( *shot_sequence, iLocalSpace.SequenceID, iLocalSpace.RootToSequenceTransform, mPanels );
        }

        FExportConverter*           mConverter;
        const FExportMarkSettings*  mMarkSettings;
        ISequencer*                 mSequencer;
        FMovieSceneSequenceID       mSequenceId;
        TArray<FExportPanel>        mPanels;
    };

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;

    FSequenceShotVisitor shot_visitor;
    shot_visitor.mConverter = this;
    shot_visitor.mSequencer = sequencer;
    shot_visitor.mSequenceId = mSequenceId;
    shot_visitor.mMarkSettings = mMarkSettings;

    // If the root sequence is a shot, the visitor won't visit anything
    UShotSequence* root_shot_sequence = Cast<UShotSequence>( sequencer->GetRootMovieSceneSequence() );
    if( root_shot_sequence )
    {
        FMovieSceneSequenceTransform transform;

        if( mMarkSettings->Marks )
            ProcessSequencerMarks( *root_shot_sequence, sequencer->GetRootTemplateID(), transform, shot_visitor.mPanels );

        if( mMarkSettings->AnimationCuts )
            ProcessAnimationCuts( *root_shot_sequence, sequencer->GetRootTemplateID(), transform, shot_visitor.mPanels );

        if( mMarkSettings->FirstFrameOfShot )
            ProcessFirstShotFrame( *root_shot_sequence, sequencer->GetRootTemplateID(), transform, shot_visitor.mPanels );
    }

    // Visit all shots
    VisitSequence( sequencer->GetRootMovieSceneSequence(), params, shot_visitor );

    //---

    shot_visitor.mPanels.StableSort( []( const FExportPanel& iA, const FExportPanel& iB ) { return iA.GlobalFrame < iB.GlobalFrame; } );

    mStruct->Panels = shot_visitor.mPanels;
}

//---

#undef LOCTEXT_NAMESPACE
