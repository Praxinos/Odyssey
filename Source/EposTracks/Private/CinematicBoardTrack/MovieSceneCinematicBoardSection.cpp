// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "Generators/MovieSceneEasingCurves.h"
#include "Helpers/SectionsHelpersResize.h"
#include "MovieSceneSection.h"

//---

UMovieSceneCinematicBoardSection::UMovieSceneCinematicBoardSection()
    : UMovieSceneSubSection()
    , mResizing( -1 )
{
    SetBlendType( EMovieSceneBlendType::Absolute );

    Easing.bManualEaseIn = true;
    Easing.ManualEaseInDuration = 0;
    Easing.bManualEaseOut = true;
    Easing.ManualEaseOutDuration = 0;
}

#if WITH_EDITOR

void UMovieSceneCinematicBoardSection::PreEditChange( FProperty* PropertyAboutToChange )
{
    if( PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED( UMovieSceneCinematicBoardSection, SectionRange ) )
    {
        mSectionRangeBackup = GetTrueRange();
    }

    Super::PreEditChange( PropertyAboutToChange );
}

void UMovieSceneCinematicBoardSection::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty( PropertyChangedEvent );

    const FName PropertyName = PropertyChangedEvent.GetPropertyName();
    if( PropertyName == GET_MEMBER_NAME_CHECKED( UMovieSceneCinematicBoardSection, SectionRange ) )
    {
        auto section_range( GetTrueRange() ); // as SectionRange but 'normalized'

        if( mSectionRangeBackup.GetLowerBoundValue() == section_range.GetLowerBoundValue() && mSectionRangeBackup.GetUpperBoundValue() == section_range.GetUpperBoundValue() )
        {
        }
        else if( mSectionRangeBackup.GetLowerBoundValue() == section_range.GetLowerBoundValue() )
        {
            UMovieScene* outer_movie_scene = GetTypedOuter<UMovieScene>();
            int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );
            UMovieSceneTrack* outer_track = GetTypedOuter<UMovieSceneTrack>();

            SetRange( SectionsHelpersResize::GetValidRangeTrailing( outer_track->GetAllSections(), this, section_range.GetUpperBoundValue(), IntervalSnapThreshold ) );

            mResizing = 0;
            Resizing();
        }
        else if( mSectionRangeBackup.GetUpperBoundValue() == section_range.GetUpperBoundValue() )
        {
            UMovieScene* outer_movie_scene = GetTypedOuter<UMovieScene>();
            int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );
            UMovieSceneTrack* outer_track = GetTypedOuter<UMovieSceneTrack>();

            SetRange( SectionsHelpersResize::GetValidRangeLeading( outer_track->GetAllSections(), this, section_range.GetLowerBoundValue(), IntervalSnapThreshold ) );

            mResizing = 0;
            Resizing();
        }
        else if( mSectionRangeBackup.Size<FFrameNumber>() == GetTrueRange().Size<FFrameNumber>() )
        {
            mMoving = 0;
            Moving();
        }

        if( UMovieSceneCinematicBoardTrack* Track = GetTypedOuter<UMovieSceneCinematicBoardTrack>() )
        {
            Track->OnSectionMoved( *this, EPropertyChangeType::ValueSet );
        }
    }
}

#endif

//---

void
UMovieSceneCinematicBoardSection::StartResizing()
{
    mResizing = 0;

    mSectionRangeBackup = GetTrueRange();
}
void
UMovieSceneCinematicBoardSection::Resizing()
{
    if( mResizing < 0 )
        return;

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

//---

bool
UMovieSceneCinematicBoardSection::GuessStartMoving( TRange<FFrameNumber>& oRangeBackup )
{
    if( IsMoving() )
        return false;

    UMovieSceneTrack* outer_track = GetTypedOuter<UMovieSceneTrack>();
    auto all_sections = outer_track->GetAllSections();

    TArray<UMovieSceneSection*> sections_without_selected;
    for( int i = 0; i < all_sections.Num(); i++ )
    {
        if( all_sections[i] == this )
            continue;

        sections_without_selected.Add( all_sections[i] );
    }

    if( !sections_without_selected.Num() )
    {
        StartMoving(); // Backup value is not valid, but shouldn't be a problem for this case, where there is only 1 section and it is moving
        mSectionRangeBackup = GetTrueRange(); // wrong, but doesn't impact after as there is no previous/next sections
        oRangeBackup = mSectionRangeBackup;

        return true;
    }

    TRange<FFrameNumber> gap( TRange<FFrameNumber>::Empty() );

    for( int i = 0; i < sections_without_selected.Num(); i++ )
    {
        UMovieSceneSection* current_section = sections_without_selected[i];
        TRangeBound<FFrameNumber> current_upper_bound = current_section->GetTrueRange().GetUpperBound();

        if( !sections_without_selected.IsValidIndex( i + 1 ) )
            continue;

        UMovieSceneSection* next_section = sections_without_selected[i + 1];
        TRangeBound<FFrameNumber> next_lower_bound = next_section->GetTrueRange().GetLowerBound();
        if( current_upper_bound.GetValue() != next_lower_bound.GetValue() )
        {
            gap = TRange<FFrameNumber>( TRangeBound<FFrameNumber>::FlipInclusion( current_upper_bound ), TRangeBound<FFrameNumber>::FlipInclusion( next_lower_bound ) );
        }
    }

    if( !gap.IsEmpty() )
    {
        StartMoving();
        mSectionRangeBackup = gap;
        oRangeBackup = mSectionRangeBackup;

        return true;
    }

    TRange<FFrameNumber> first_range( 0, sections_without_selected[0]->GetInclusiveStartFrame() );
    if( !first_range.IsEmpty() && first_range.Size<FFrameNumber>() >= GetTrueRange().Size<FFrameNumber>() )
    {
        StartMoving();
        mSectionRangeBackup = first_range;
        oRangeBackup = mSectionRangeBackup;

        return true;
    }

    TRange<FFrameNumber> last_range( sections_without_selected.Last()->GetExclusiveEndFrame(), sections_without_selected.Last()->GetExclusiveEndFrame() + GetTrueRange().Size<FFrameNumber>() );
    StartMoving();
    mSectionRangeBackup = last_range;
    oRangeBackup = mSectionRangeBackup;

    return true;
}

void
UMovieSceneCinematicBoardSection::StartMoving()
{
    mMoving = 0;

    mSectionRangeBackup = GetTrueRange();
}
void
UMovieSceneCinematicBoardSection::Moving()
{
    if( mMoving < 0 )
        return;

    mMoving++;
}
void
UMovieSceneCinematicBoardSection::StopMoving()
{
    mMoving = -1;

    mSectionRangeBackup = TRange<FFrameNumber>::Empty();
}
bool
UMovieSceneCinematicBoardSection::IsMoving() const
{
    return mMoving > 0;
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
