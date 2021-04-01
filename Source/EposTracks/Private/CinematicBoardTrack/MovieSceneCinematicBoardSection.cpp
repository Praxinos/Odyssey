// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "EposMovieSceneSequence.h"
#include "Generators/MovieSceneEasingCurves.h"
#include "Helpers/SectionsHelpersResize.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"

//---

UMovieSceneCinematicBoardSection::UMovieSceneCinematicBoardSection()
    : UMovieSceneSubSection()
    , mResizing( -1 )
    , mLeadingResizing( false )
    , mTrailingResizing( false )
    , mMoving( -1 )
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
            ResizeTrailingEdge( section_range.GetUpperBoundValue() );
            if( IsResizingTrailing() )
            {
                mResizing = 0;
                Resizing();
            }
            else
            {
                SetRange( mSectionRangeBackup );
            }
        }
        else if( mSectionRangeBackup.GetUpperBoundValue() == section_range.GetUpperBoundValue() )
        {
            ResizeLeadingEdge( section_range.GetLowerBoundValue() );
            if( IsResizingLeading() )
            {
                mResizing = 0;
                Resizing();
            }
            else
            {
                SetRange( mSectionRangeBackup );
            }
        }
        else if( UE::MovieScene::DiscreteSize( mSectionRangeBackup ) == UE::MovieScene::DiscreteSize( GetTrueRange() ) )
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
UMovieSceneCinematicBoardSection::ResizeLeadingEdge( FFrameNumber iNewFrame )
{
    if( !IsResizableLeadingEdge() )
        return;

    mLeadingResizing = true;

    UMovieScene* outer_movie_scene = GetTypedOuter<UMovieScene>();
    int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );
    UMovieSceneTrack* outer_track = GetTypedOuter<UMovieSceneTrack>();

    auto new_range = SectionsHelpersResize::GetValidRangeLeading( outer_track->GetAllSections(), this, iNewFrame, IntervalSnapThreshold );
    //SetRange( new_range );
    FFrameNumber diff = new_range.GetLowerBoundValue() - GetInclusiveStartFrame();
    MoveSection( diff );
}

void
UMovieSceneCinematicBoardSection::ResizeTrailingEdge( FFrameNumber iNewFrame )
{
    if( !IsResizableTrailingEdge() )
        return;

    mTrailingResizing = true;

    UMovieScene* outer_movie_scene = GetTypedOuter<UMovieScene>();
    int32 IntervalSnapThreshold = FMath::RoundToInt( ( outer_movie_scene->GetTickResolution() / outer_movie_scene->GetDisplayRate() ).AsDecimal() );
    UMovieSceneTrack* outer_track = GetTypedOuter<UMovieSceneTrack>();

    auto new_range = SectionsHelpersResize::GetValidRangeTrailing( outer_track->GetAllSections(), this, iNewFrame, IntervalSnapThreshold );
    SetRange( new_range );
}

bool
UMovieSceneCinematicBoardSection::IsSequenceResizable( const UMovieSceneSection* iSection ) const
{
    const UMovieSceneSubSection* subsection = Cast<const UMovieSceneSubSection>( iSection );
    if( !subsection )
        return false;

    const UEposMovieSceneSequence* subsequence = Cast<UEposMovieSceneSequence>( subsection->GetSequence() );
    if( !subsequence )
        return true;

    return subsequence->IsResizable();
}

bool
UMovieSceneCinematicBoardSection::IsResizableLeadingEdge()
{
    UMovieSceneTrack* outer_track = GetTypedOuter<UMovieSceneTrack>();
    auto sections = outer_track->GetAllSections();

    int32 current_index = INDEX_NONE;
    if( !sections.Find( this, current_index ) )
        return false;
    if( !sections.IsValidIndex( current_index - 1 ) )
        return false;

    return IsSequenceResizable( sections[current_index - 1] );
}
bool
UMovieSceneCinematicBoardSection::IsResizableTrailingEdge()
{
    return IsSequenceResizable( this );
}

bool
UMovieSceneCinematicBoardSection::IsResizingLeading() const
{
    return mLeadingResizing;
}

bool
UMovieSceneCinematicBoardSection::IsResizingTrailing() const
{
    return mTrailingResizing;
}

void
UMovieSceneCinematicBoardSection::StartResizing()
{
    mResizing = 0;
    mLeadingResizing = false;
    mTrailingResizing = false;

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
    mLeadingResizing = false;
    mTrailingResizing = false;
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
    if( !first_range.IsEmpty() && UE::MovieScene::DiscreteSize( first_range ) >= UE::MovieScene::DiscreteSize( GetTrueRange() ) )
    {
        StartMoving();
        mSectionRangeBackup = first_range;
        oRangeBackup = mSectionRangeBackup;

        return true;
    }

    TRange<FFrameNumber> last_range( sections_without_selected.Last()->GetExclusiveEndFrame(), sections_without_selected.Last()->GetExclusiveEndFrame() + UE::MovieScene::DiscreteSize( GetTrueRange() ) );
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

float
UMovieSceneCinematicBoardSection::GetWidgetHeight() const
{
    return mWidgetHeight.Get();
}

/** Set the height of this track's rows */
void
UMovieSceneCinematicBoardSection::SetWidgetHeight( TAttribute<float> iWidgetHeight )
{
    mWidgetHeight = iWidgetHeight;
}

#endif
