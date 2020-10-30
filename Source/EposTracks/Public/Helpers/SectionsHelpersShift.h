// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;

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

class EPOSTRACKS_API MovieSceneHelpersShift
{
public:
    static FShiftResult GetShiftInfo( TArray< UMovieSceneSection* > iSections, FFrameNumber iStartTime, FFrameNumber iDuration );

    static void ShiftFollowingSections( TArray< UMovieSceneSection* > iSections, const UMovieSceneSection* iNewSection, FShiftResult iShiftResult );
    static void ShiftFollowingSectionsAfterDelete( TArray< UMovieSceneSection* > iSections, const UMovieSceneSection* iNewSection );
};
