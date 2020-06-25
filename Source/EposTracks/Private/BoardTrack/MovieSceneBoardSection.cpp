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
    return BoardDisplayName;
}

void
UMovieSceneBoardSection::SetBoardDisplayName( const FString& InBoardDisplayName )
{
    if( TryModify() )
    {
        BoardDisplayName = InBoardDisplayName;
    }
}

//---

#if WITH_EDITORONLY_DATA

float
UMovieSceneBoardSection::GetThumbnailReferenceOffset() const
{
    return ThumbnailReferenceOffset;
}

void
UMovieSceneBoardSection::SetThumbnailReferenceOffset( float InNewOffset )
{
    Modify();
    ThumbnailReferenceOffset = InNewOffset;
}

#endif
