// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportConverter.h"

#include "AssetToolsModule.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Compilation/MovieSceneCompiledDataManager.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "ISequencer.h"
#include "MovieSceneSequenceVisitor.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
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
FExportConverter::ProcessDrawings( UShotSequence& iShotSequence, FMovieSceneSequenceIDRef iSequenceId, const FMovieSceneSequenceTransform& iRootToSequenceTransform, TArray<FExportPanel>& ioPanels ) const
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    TArray<APlaneActor*> planes;
    TArray<FGuid> plane_bindings;
    /*int32 num_planes =*/ ShotSequenceHelpers::GetAllPlanes( *sequencer, &iShotSequence, iSequenceId, EGetPlane::kAll, &planes, &plane_bindings );

    for( auto plane_binding : plane_bindings )
    {
        TArray<FDrawing> drawings = ShotSequenceHelpers::GetAllDrawings( *sequencer, &iShotSequence, iSequenceId, plane_binding );
        for( auto drawing : drawings )
        {
            FFrameNumber frame;
            drawing.mChannel->GetKeyTimes( TArrayView<const FKeyHandle>( &drawing.mKeyHandle, 1 ), TArrayView<FFrameNumber>( &frame, 1 ) );

            FMovieSceneInverseSequenceTransform localToRootTransform = iRootToSequenceTransform.Inverse();
            TOptional<FFrameTime> time_in_root = localToRootTransform.TryTransformTime( frame );
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

                if( !existing_panel->mSourceDrawing.IsSet() )
                    existing_panel->mSourceDrawing = FExportPanelSourceDrawing();

                existing_panel->mSourceDrawing.GetValue().mDrawings.Add( { drawing, plane_binding } );
            }
            else
            {
                FExportPanel panel;
                panel.GlobalFrame = frame_in_root;
                panel.mSequence = &iShotSequence;
                panel.mSequenceId = iSequenceId;
                FExportPanelSourceDrawing source_drawing;
                source_drawing.mDrawings.Add( { drawing, plane_binding } );
                panel.mSourceDrawing = source_drawing;

                ioPanels.Add( panel );
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

            if( mMarkSettings->Drawings )
                mConverter->ProcessDrawings( *shot_sequence, iLocalSpace.SequenceID, iLocalSpace.RootToSequenceTransform, mPanels );

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

        if( mMarkSettings->Drawings )
            ProcessDrawings( *root_shot_sequence, sequencer->GetRootTemplateID(), transform, shot_visitor.mPanels );

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
