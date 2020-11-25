// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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

class EPOSTRACKS_API SectionsHelpersShift
{
public:
    static FShiftResult GetShiftInfo( TArray< UMovieSceneSection* > iSections, FFrameNumber iStartTime, FFrameNumber iDuration );

    static void ShiftFollowingSections( TArray< UMovieSceneSection* > iSections, const UMovieSceneSection* iNewSection, FShiftResult iShiftResult );

    /** Move all sections to make consecutives each other (previous end bound == next start bound) by the order inside the array */
    static void OrganizeSections( TArray< UMovieSceneSection* > iSections );
};
