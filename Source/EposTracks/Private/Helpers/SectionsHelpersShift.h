// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;
class UMovieSceneSequence;

struct FShiftResult
{
    TRange<FFrameNumber>    mNewRange; // Like TrueRange() [I,E)
    bool                    mFillGap;
    TRange<FFrameNumber>    mGap;

    FShiftResult()
        : mNewRange( TRange<FFrameNumber>::Empty() )
        , mFillGap( false )
        , mGap( TRange<FFrameNumber>::Empty() )
    {
    }
};

class EPOSTRACKS_API SectionsHelpersShift
{
public:
    static FShiftResult GetShiftInfo( TArray< UMovieSceneSection* > iSections, UMovieSceneSequence* iSequence, FFrameNumber iStartTime, TOptional<int32> iDuration );

    static void ShiftFollowingSections( TArray< UMovieSceneSection* > iSections, const UMovieSceneSection* iNewSection, FShiftResult iShiftResult );
};
