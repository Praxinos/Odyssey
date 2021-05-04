// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "MovieSceneSequenceID.h"

class UMovieScene;
class UMovieSceneSequence;
class UMovieSceneSubSection;
class UMovieSceneTrack;
class IMovieScenePlayer;

class EPOSSEQUENCE_API BoardSequenceHelpers
{
public:
    struct FInnerSequenceResult
    {
        UMovieSceneSequence*    mInnerSequence = nullptr;
        FMovieSceneSequenceID   mInnerSequenceId = MovieSceneSequenceID::Invalid;
        UMovieScene*            mInnerMovieScene = nullptr;
        FFrameTime              mInnerTime = 0;
    };
    static FInnerSequenceResult GetInnerSequence( IMovieScenePlayer& iSequencer, const UMovieSceneSubSection& iSubSection, FMovieSceneSequenceIDRef iSequenceId );
    static FInnerSequenceResult GetInnerSequence( IMovieScenePlayer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceId, const FFrameNumber& iFrameNumber );

public:
    static TArray<FFrameTime> GetCameraTransformTimesRecursive( const UMovieSceneSubSection& iSubSection );
};
