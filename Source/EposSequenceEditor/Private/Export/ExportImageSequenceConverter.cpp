// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportImageSequenceConverter.h"

#include "AssetToolsModule.h"
#include "ISequencer.h"
#include "MovieSceneSequenceVisitor.h"
#include "Sections/MovieSceneSubSection.h"

#include "Board/BoardSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposSequenceHelpers.h"
#include "NamingConvention.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "ExportImageSequenceConverter"

//---

FExportImageSequenceConverter::FExportImageSequenceConverter( TWeakPtr<ISequencer> iSequencer, const UMovieSceneSequence* iRootSequence, FExportImageSequenceStruct* oImageSequenceStruct )
    : mSequencer( iSequencer )
    , mImageSequenceStruct( oImageSequenceStruct )
    , mRootSequence( iRootSequence )
{
    check( iSequencer.Pin()->GetFocusedMovieSceneSequence() == mRootSequence );

    Convert();
}

void
FExportImageSequenceConverter::Convert()
{
    ISequencer* sequencer = mSequencer.Pin().Get();

    struct FSequenceShotVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitSubSequence( UMovieSceneSequence* iSequence, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
            UShotSequence* shot_sequence = Cast<UShotSequence>( iSequence );
            if( !shot_sequence )
                return;

            TArray<FFrameNumber> frames = ShotSequenceHelpers::GetAllDrawingTimes( *mSequencer, iSequence, iLocalSpace.SequenceID, EGetPlane::kAll );

            TSet<FFrameNumber> frames_set;
            for( auto frame : frames )
            {
                FFrameNumber frame_in_root = ( frame * iLocalSpace.RootToSequenceTransform.InverseLinearOnly() ).GetFrame();
                frames_set.Add( frame_in_root );
            }

            mFrames.Append( frames_set );
        }

        ISequencer*             mSequencer;
        TSet<FFrameNumber>      mFrames;
    };

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitMasterTracks = true;
    params.bVisitSubSequences = true;

    FSequenceShotVisitor shot_visitor;
    shot_visitor.mSequencer = sequencer;

    // Visit all shots
    VisitSequence( sequencer->GetRootMovieSceneSequence(), params, shot_visitor );

    //---

    for( auto frame : shot_visitor.mFrames )
    {
        FExportImageSequencePanel panel;
        panel.GlobalFrame = frame;

        mImageSequenceStruct->Panels.Add( panel );
    }
}

//---

#undef LOCTEXT_NAMESPACE
