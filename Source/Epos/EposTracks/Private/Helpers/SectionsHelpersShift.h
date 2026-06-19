// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
