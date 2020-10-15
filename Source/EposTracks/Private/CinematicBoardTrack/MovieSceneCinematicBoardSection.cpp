// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "MovieSceneHelpersResize.h"

//---

UMovieSceneCinematicBoardSection::UMovieSceneCinematicBoardSection()
    : UMovieSceneSubSection()
{
}

#if WITH_EDITOR

void UMovieSceneCinematicBoardSection::PreEditChange( FProperty* PropertyAboutToChange )
{
    if( PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED( UMovieSceneCinematicBoardSection, SectionRange ) )
    {
        mSectionRangeBackup = SectionRange;
    }

    Super::PreEditChange( PropertyAboutToChange );
}

void UMovieSceneCinematicBoardSection::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    const FName PropertyName = PropertyChangedEvent.GetPropertyName();
    if( PropertyName == GET_MEMBER_NAME_CHECKED( UMovieSceneCinematicBoardSection, SectionRange ) )
    {
        if( mSectionRangeBackup.GetLowerBound().GetValue() == SectionRange.GetLowerBound().GetValue() && mSectionRangeBackup.GetUpperBound().GetValue() == SectionRange.GetUpperBound().GetValue() )
        {
        }
        else if( mSectionRangeBackup.GetLowerBound().GetValue() == SectionRange.GetLowerBound().GetValue() )
        {
            UMovieScene* outer_movie_scene = GetTypedOuter<UMovieScene>();
            int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );

            SetRange( MovieSceneHelpersResize::GetValidRangeTrailing( outer_movie_scene->GetAllSections(), this, SectionRange.GetUpperBound().GetValue(), IntervalSnapThreshold ) );

            mResizing = 0;
            Resizing();
        }
        else if( mSectionRangeBackup.GetUpperBound().GetValue() == SectionRange.GetUpperBound().GetValue() )
        {
            UMovieScene* outer_movie_scene = GetTypedOuter<UMovieScene>();
            int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );

            SetRange( MovieSceneHelpersResize::GetValidRangeLeading( outer_movie_scene->GetAllSections(), this, SectionRange.GetLowerBound().GetValue(), IntervalSnapThreshold ) );

            mResizing = 0;
            Resizing();
        }

        if( UMovieSceneCinematicBoardTrack* Track = GetTypedOuter<UMovieSceneCinematicBoardTrack>() )
        {
            Track->OnSectionMoved( *this, EPropertyChangeType::ValueSet );
        }
    }
}

#endif

void
UMovieSceneCinematicBoardSection::StartResizing()
{
    mResizing = 0;

    mSectionRangeBackup = GetTrueRange();
}
void
UMovieSceneCinematicBoardSection::Resizing()
{
    mResizing++;
}
void
UMovieSceneCinematicBoardSection::StopResizing()
{
    mResizing = -1;

    mSectionRangeBackup = TRange<FFrameNumber>::Empty();
}
bool
UMovieSceneCinematicBoardSection::IsResizing() const
{
    return mResizing > 0;
}

FMovieSceneFrameRange
UMovieSceneCinematicBoardSection::GetTrueRangeBackup() const
{
    return mSectionRangeBackup;
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
