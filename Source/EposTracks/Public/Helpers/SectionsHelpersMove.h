// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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

    FMoveResult()
        : mForward( false )
        , mBackward( false )
        , mReferenceFrame( 0 )
        , mInitialGap( TRange<FFrameNumber>::Empty() )
        , mInitialGapMiddle( 0 )
        , mInitialGapSize( 0 )
        , mSectionsBeforeGap()
        , mSectionsAfterGap()
    {
    }
};

class EPOSTRACKS_API MovieSceneHelpersMove
{
public:
    static FMoveResult GetMoveInfo( TArray< UMovieSceneSection* > iSections, TRange<FFrameNumber> iPreviousMove, TRange<FFrameNumber> iLastGapMove, const UMovieSceneSection* iSection );

    static void FixMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber>* ioLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult );
    static void FixPostMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber> iLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult );
};
