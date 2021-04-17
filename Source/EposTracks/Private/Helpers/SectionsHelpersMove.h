// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;

struct FMoveResult
{
    bool                        mForward;
    bool                        mBackward;
    FFrameNumber                mReferenceFrame;

    TRange<FFrameNumber>        mInitialGap;
    FFrameNumber                mInitialGapMiddle;
    FFrameNumber                mInitialGapSize;
    TArray<UMovieSceneSection*> mSectionsBeforeGap;
    TArray<UMovieSceneSection*> mSectionsAfterGap;

    FMoveResult();
};

class EPOSTRACKS_API SectionsHelpersMove
{
public:
    static FMoveResult GetMoveInfo( TArray< UMovieSceneSection* > iSections, TRange<FFrameNumber> iPreviousMove, TRange<FFrameNumber> iLastGapMove, const UMovieSceneSection* iSection );

    static void FixMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber>* ioLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult );
    static void FixPostMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber> iLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult );
};
