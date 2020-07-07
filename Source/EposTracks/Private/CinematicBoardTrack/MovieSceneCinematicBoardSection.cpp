// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"

//---

UMovieSceneCinematicBoardSection::UMovieSceneCinematicBoardSection()
    : UMovieSceneSubSection()
{
}

//---

FString
UMovieSceneCinematicBoardSection::GetBoardDisplayName() const
{
    return mBoardDisplayName;
}

void
UMovieSceneCinematicBoardSection::SetBoardDisplayName( const FString& iBoardDisplayName )
{
    if( TryModify() )
    {
        mBoardDisplayName = iBoardDisplayName;
    }
}

//---

#if WITH_EDITORONLY_DATA

float
UMovieSceneCinematicBoardSection::GetThumbnailReferenceOffset() const
{
    return mThumbnailReferenceOffset;
}

void
UMovieSceneCinematicBoardSection::SetThumbnailReferenceOffset( float iNewOffset )
{
    Modify();
    mThumbnailReferenceOffset = iNewOffset;
}

#endif
