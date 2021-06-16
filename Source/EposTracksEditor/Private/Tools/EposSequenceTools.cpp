// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/EposSequenceTools.h"

#include "ISequencer.h"
#include "MovieScene.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"

#define LOCTEXT_NAMESPACE "EposSequenceTools"

ShotSequenceTools::cTemporarySwitchInner::cTemporarySwitchInner( ISequencer& iSequencer, FMovieSceneSequenceIDRef iInnerID )
    : mSequencer( iSequencer )
    , mOriginalId()
    , mOriginalGlobalTime()
{
    mOriginalId = mSequencer.GetFocusedTemplateID();
    if( iInnerID == mOriginalId )
        return;

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mOriginalGlobalTime = ConvertFrameTime( mSequencer.GetGlobalTime().Time, tick_resolution, display_rate );

    UMovieSceneSubSection* subsection = mSequencer.FindSubSection( iInnerID );
    check( subsection );
    mSequencer.FocusSequenceInstance( *subsection );
}

ShotSequenceTools::cTemporarySwitchInner::~cTemporarySwitchInner()
{
    FMovieSceneSequenceID focused_id = mSequencer.GetFocusedTemplateID();
    if( focused_id == mOriginalId )
        return;

    if( mOriginalId == MovieSceneSequenceID::Root )
    {
        mSequencer.ResetToNewRootSequence( *mSequencer.GetRootMovieSceneSequence() );
    }
    else
    {
        UMovieSceneSubSection* subsection = mSequencer.FindSubSection( mOriginalId );
        check( subsection );
        mSequencer.FocusSequenceInstance( *subsection );
    }

    FFrameRate display_rate = mSequencer.GetFocusedDisplayRate();
    FFrameRate tick_resolution = mSequencer.GetFocusedTickResolution();
    mSequencer.SetGlobalTime( ConvertFrameTime( mOriginalGlobalTime, display_rate, tick_resolution ) );
}

#undef LOCTEXT_NAMESPACE
