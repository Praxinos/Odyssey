// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "EposSequenceHelpers.h"

/** Editor queries and channel proxies for animation cuts. */
class EPOSTRACKSEDITOR_API FEposTracksEditorHelpers
{
public:
    static TArray<FFrameNumber>             GetAllAnimationCutTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, const FFrameNumber& iFrameNumber );
    static TMap<FGuid, FChannelProxyBySectionMap>   BuildAnimationsTimelineChannelProxy( IMovieScenePlayer& iPlayer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceID );
    static TArray<FFrameNumber>                         GetAllAnimationCutTimes( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static TMap<FGuid, FChannelProxyBySectionMap>       BuildAnimationsTimelineChannelProxy( IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
};
