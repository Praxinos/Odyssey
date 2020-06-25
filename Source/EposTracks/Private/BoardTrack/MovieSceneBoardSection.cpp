// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "BoardTrack/MovieSceneBoardSection.h"

//---

UMovieSceneBoardSection::UMovieSceneBoardSection() 
    : UMovieSceneSubSection()
{
}

//---

FString
UMovieSceneBoardSection::GetBoardDisplayName() const
{
    return mBoardDisplayName;
}

void
UMovieSceneBoardSection::SetBoardDisplayName( const FString& iBoardDisplayName )
{
    if( TryModify() )
    {
        mBoardDisplayName = iBoardDisplayName;
    }
}

//---

#if WITH_EDITORONLY_DATA

float
UMovieSceneBoardSection::GetThumbnailReferenceOffset() const
{
    return mThumbnailReferenceOffset;
}

void
UMovieSceneBoardSection::SetThumbnailReferenceOffset( float iNewOffset )
{
    Modify();
    mThumbnailReferenceOffset = iNewOffset;
}

#endif
