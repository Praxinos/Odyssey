// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportConverter.h"

#include "AssetToolsModule.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "ISequencer.h"
#include "MovieSceneSequenceVisitor.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ExportConverter"

//---

FExportConverter::FExportConverter( TWeakPtr<ISequencer> iSequencer, const UMovieSceneSequence* iRootSequence, const FExportMarkSettings* iMarkSettings, FExportStruct* oStruct )
    : mSequencer( iSequencer )
    , mRootSequence( iRootSequence )
    , mMarkSettings( iMarkSettings )
    , mStruct( oStruct )
{
    check( iSequencer.Pin()->GetFocusedMovieSceneSequence() == mRootSequence );

    Convert();
}

void
FExportConverter::Convert()
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    //---

    struct FSequenceShotVisitor
        : UE::MovieScene::ISequenceVisitor
    {

        void ProcessSequencerMarks( UMovieSceneSequence* iSequence, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UShotSequence* shot_sequence = Cast<UShotSequence>( iSequence );
            if( !shot_sequence )
                return;

            //---

            TArray<FMovieSceneMarkedFrame> marks = iSequence->GetMovieScene()->GetMarkedFrames();
            for( auto mark : marks )
            {
                FFrameNumber frame_in_root = ( mark.FrameNumber * iLocalSpace.RootToSequenceTransform.InverseLinearOnly() ).GetFrame();

                FExportPanel* existing_panel = mPanels.FindByPredicate( [frame_in_root]( const FExportPanel& iElement )
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
                    panel.mSequence = iSequence;
                    panel.mSourceMark = { mark };

                    mPanels.Add( panel );
                }
            }

        }

        void ProcessDrawings( UMovieSceneSequence* iSequence, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UShotSequence* shot_sequence = Cast<UShotSequence>( iSequence );
            if( !shot_sequence )
                return;

            //---

            TArray<APlaneActor*> planes;
            TArray<FGuid> plane_bindings;
            /*int32 num_planes =*/ ShotSequenceHelpers::GetAllPlanes( *mSequencer, iSequence, iLocalSpace.SequenceID, EGetPlane::kAll, &planes, &plane_bindings );

            for( auto plane_binding : plane_bindings )
            {
                TArray<FDrawing> drawings = ShotSequenceHelpers::GetAllDrawings( *mSequencer, iSequence, iLocalSpace.SequenceID, plane_binding );
                for( auto drawing : drawings )
                {
                    FFrameNumber frame;
                    drawing.mChannel->GetKeyTimes( TArrayView<const FKeyHandle>( &drawing.mKeyHandle, 1 ), TArrayView<FFrameNumber>( &frame, 1 ) );

                    FFrameNumber frame_in_root = ( frame * iLocalSpace.RootToSequenceTransform.InverseLinearOnly() ).GetFrame();

                    FExportPanel* existing_panel = mPanels.FindByPredicate( [frame_in_root]( const FExportPanel& iElement )
                                                                            {
                                                                                return iElement.GlobalFrame == frame_in_root;
                                                                            } );
                    if( existing_panel )
                    {
                        check( existing_panel->mSequence == iSequence );

                        if( !existing_panel->mSourceDrawing.IsSet() )
                            existing_panel->mSourceDrawing = FExportPanelSourceDrawing();

                        existing_panel->mSourceDrawing.GetValue().mDrawings.Add( { drawing, plane_binding } );
                    }
                    else
                    {
                        FExportPanel panel;
                        panel.GlobalFrame = frame_in_root;
                        panel.mSequence = iSequence;
                        FExportPanelSourceDrawing source_drawing;
                        source_drawing.mDrawings.Add( { drawing, plane_binding } );
                        panel.mSourceDrawing = source_drawing;

                        mPanels.Add( panel );
                    }
                }
            }

        }

        void ProcessFirstShotFrame( UMovieSceneSequence* iSequence, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UShotSequence* shot_sequence = Cast<UShotSequence>( iSequence );
            if( !shot_sequence )
                return;

            //---

            TRange<FFrameNumber> playback_range = iSequence->GetMovieScene()->GetPlaybackRange();

            FFrameNumber frame_in_root = ( playback_range.GetLowerBoundValue() * iLocalSpace.RootToSequenceTransform.InverseLinearOnly() ).GetFrame();

            FExportPanel* existing_panel = mPanels.FindByPredicate( [frame_in_root]( const FExportPanel& iElement )
                                                                    {
                                                                        return iElement.GlobalFrame == frame_in_root;
                                                                    } );
            if( existing_panel )
            {
                check( existing_panel->mSequence == iSequence );

                // If there is already in the panel list, nothing to do as we store only the frame for this case
            }
            else
            {
                FExportPanel panel;
                panel.GlobalFrame = frame_in_root;
                panel.mSequence = iSequence;

                mPanels.Add( panel );
            }
        }

        virtual void VisitSubSequence( UMovieSceneSequence* iSequence, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            if( mMarkSettings->Marks )
                ProcessSequencerMarks( iSequence, iGuid, iLocalSpace );

            if( mMarkSettings->Drawings )
                ProcessDrawings( iSequence, iGuid, iLocalSpace );

            if( mMarkSettings->FirstFrameOfShot )
                ProcessFirstShotFrame( iSequence, iGuid, iLocalSpace );
        }

        const FExportMarkSettings*  mMarkSettings;
        ISequencer*                 mSequencer;
        TArray<FExportPanel>        mPanels;
    };

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;

    FSequenceShotVisitor shot_visitor;
    shot_visitor.mSequencer = sequencer;
    shot_visitor.mMarkSettings = mMarkSettings;

    // Visit all shots
    VisitSequence( sequencer->GetRootMovieSceneSequence(), params, shot_visitor );

    //---

    shot_visitor.mPanels.StableSort( []( const FExportPanel& iA, const FExportPanel& iB ) { return iA.GlobalFrame < iB.GlobalFrame; } );

    mStruct->Panels = shot_visitor.mPanels;
}

//---

#undef LOCTEXT_NAMESPACE
