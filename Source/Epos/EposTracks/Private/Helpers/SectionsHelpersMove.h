// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

    static TArray< UMovieSceneSection* > GetOrderedSections( const TArray< UMovieSceneSection* > iSections );

    static void FixMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber>* ioLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult );
    static void FixPostMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber> iLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult );
};
