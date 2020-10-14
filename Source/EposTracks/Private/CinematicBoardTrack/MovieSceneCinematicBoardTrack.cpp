// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"

#include "MovieSceneSequence.h"
#include "MovieSceneCommonHelpers.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "Compilation/MovieSceneCompilerRules.h"

#include "MovieSceneHelpersShift.h"


#define LOCTEXT_NAMESPACE "MovieSceneCinematicBoardTrack"


/* UMovieSceneSubTrack interface
 *****************************************************************************/
UMovieSceneCinematicBoardTrack::UMovieSceneCinematicBoardTrack( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 0, 0, 0, 127 );
#endif
}

UMovieSceneSubSection*
UMovieSceneCinematicBoardTrack::AddSequence( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration )
{
    return AddSequenceOnRow( iSequence, iStartTime, iDuration, INDEX_NONE );
}

UMovieSceneSubSection*
UMovieSceneCinematicBoardTrack::AddSequenceOnRow( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration, int32 iRowIndex )
{
    FShiftResult shift_result = MovieSceneHelpersShift::GetShiftInfo( Sections, iStartTime, iDuration );

    //---

    UMovieSceneSubSection* newSection = UMovieSceneSubTrack::AddSequenceOnRow( iSequence, shift_result.mNewRange.GetLowerBoundValue(), MovieSceneHelpersShift::RangeSize( shift_result.mNewRange ).Value, iRowIndex );

    UMovieSceneCinematicBoardSection* newBoardSection = Cast<UMovieSceneCinematicBoardSection>( newSection );

#if WITH_EDITOR

    if( iSequence != nullptr )
    {
        newBoardSection->SetBoardDisplayName( iSequence->GetDisplayName().ToString() );
    }

#endif

    // When a new sequence is added, sort all sequences to ensure they are in the correct order
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    // Once sequences are sorted fixup the surrounding sequences to fix any gaps
    MovieSceneHelpersShift::ShiftFollowingSections( Sections, newSection, shift_result );

    // Should be done again as after the first one, at least 2 sections (new one and the one at this place) have the same start
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    return newSection;
}

/* UMovieSceneTrack interface
 *****************************************************************************/

void
UMovieSceneCinematicBoardTrack::AddSection( UMovieSceneSection& ioSection )
{
    if( ioSection.IsA<UMovieSceneCinematicBoardSection>() )
    {
        Sections.Add( &ioSection );
    }
}

bool
UMovieSceneCinematicBoardTrack::SupportsType( TSubclassOf<UMovieSceneSection> iSectionClass ) const
{
    return iSectionClass == UMovieSceneCinematicBoardSection::StaticClass();
}


UMovieSceneSection*
UMovieSceneCinematicBoardTrack::CreateNewSection()
{
    return NewObject<UMovieSceneCinematicBoardSection>( this, NAME_None, RF_Transactional );
}

void
UMovieSceneCinematicBoardTrack::RemoveSection( UMovieSceneSection& ioSection )
{
    Sections.Remove( &ioSection );
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    MovieSceneHelpersShift::ShiftFollowingSectionsAfterDelete( Sections, &ioSection );

    // @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?  
}

void
UMovieSceneCinematicBoardTrack::RemoveSectionAt( int32 iSectionIndex )
{
    UMovieSceneSection* deleted_section = Sections[iSectionIndex];

    Sections.RemoveAt( iSectionIndex );
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    MovieSceneHelpersShift::ShiftFollowingSectionsAfterDelete( Sections, deleted_section ); // maybe deleted_section is not need, just remove all the gap ?
}

bool
UMovieSceneCinematicBoardTrack::SupportsMultipleRows() const
{
    return true;
}

FMovieSceneTrackSegmentBlenderPtr
UMovieSceneCinematicBoardTrack::GetTrackSegmentBlender() const
{
    // Apply a high pass filter to overlapping sections such that only the highest row in a track wins
    struct FCinematicBoardTrackRowBlender : FMovieSceneTrackSegmentBlender
    {
        virtual void Blend( FSegmentBlendData& ioBlendData ) const override
        {
            MovieSceneSegmentCompiler::ChooseLowestRowIndex( ioBlendData );
        }
    };
    return FCinematicBoardTrackRowBlender();
}

FMovieSceneTrackRowSegmentBlenderPtr
UMovieSceneCinematicBoardTrack::GetRowSegmentBlender() const
{
    class FCinematicRowRules : public FMovieSceneTrackRowSegmentBlender
    {
        virtual void Blend( FSegmentBlendData& ioBlendData ) const override
        {
            // Sort everything by priority, then latest start time wins
            if( ioBlendData.Num() <= 1 )
            {
                return;
            }

            ioBlendData.Sort( SortPredicate );

            int32 removeAtIndex = 0;
            // Skip over any pre/postroll sections
            while( ioBlendData.IsValidIndex( removeAtIndex ) && EnumHasAnyFlags( ioBlendData[removeAtIndex].Flags, ESectionEvaluationFlags::PreRoll | ESectionEvaluationFlags::PostRoll ) )
            {
                ++removeAtIndex;
            }

            // Skip over the first genuine evaluation if it exists
            ++removeAtIndex;

            int32 numToRemove = ioBlendData.Num() - removeAtIndex;
            if( numToRemove > 0 )
            {
                ioBlendData.RemoveAt( removeAtIndex, numToRemove, true );
            }
        }

        static bool SortPredicate( const FMovieSceneSectionData& iA, const FMovieSceneSectionData& iB )
        {
            // Always sort pre/postroll to the front of the array
            const bool prePostRollA = EnumHasAnyFlags( iA.Flags, ESectionEvaluationFlags::PreRoll | ESectionEvaluationFlags::PostRoll );
            const bool prePostRollB = EnumHasAnyFlags( iB.Flags, ESectionEvaluationFlags::PreRoll | ESectionEvaluationFlags::PostRoll );

            if( prePostRollA != prePostRollB )
            {
                return prePostRollA;
            }
            else if( prePostRollA )
            {
                return false;
            }
            else if( iA.Section->GetOverlapPriority() == iB.Section->GetOverlapPriority() )
            {
                TRangeBound<FFrameNumber> startBoundA = iA.Section->GetRange().GetLowerBound();
                return TRangeBound<FFrameNumber>::MaxLower( startBoundA, iB.Section->GetRange().GetLowerBound() ) == startBoundA;
            }
            return iA.Section->GetOverlapPriority() > iB.Section->GetOverlapPriority();
        }
    };

    return FCinematicRowRules();
}

//---

struct FMoveResult
{
    FFrameNumber                mReferenceFrame;
    TRange<FFrameNumber>        mReferenceSectionRange;

    TRange<FFrameNumber>        mInitialGap;
    FFrameNumber                mInitialGapMiddle;
    FFrameNumber                mInitialGapSize;
    TArray<UMovieSceneSection*> mSectionsBeforeGap;
    TArray<UMovieSceneSection*> mSectionsAfterGap;

    FMoveResult()
        : mReferenceFrame( 0 )
        , mReferenceSectionRange()
        , mInitialGap( TRange<FFrameNumber>::Empty() )
        , mInitialGapMiddle( 0 )
        , mInitialGapSize( 0 )
        , mSectionsBeforeGap()
        , mSectionsAfterGap()
    {
    }
};

TRange<FFrameNumber>
FindGap( TArray< UMovieSceneSection* > iSections )
{
    TRange<FFrameNumber> gap( TRange<FFrameNumber>::Empty() );

    for( int i = 0; i < iSections.Num(); i++ )
    {
        UMovieSceneSection* current_section = iSections[i];
        TRangeBound<FFrameNumber> current_upper_bound = current_section->GetTrueRange().GetUpperBound();

        if( !iSections.IsValidIndex( i + 1 ) )
            continue;

        UMovieSceneSection* next_section = iSections[i + 1];
        TRangeBound<FFrameNumber> next_lower_bound = next_section->GetTrueRange().GetLowerBound();
        if( current_upper_bound.GetValue() != next_lower_bound.GetValue() )
        {
            gap = TRange<FFrameNumber>( TRangeBound<FFrameNumber>::FlipInclusion( current_upper_bound ), TRangeBound<FFrameNumber>::FlipInclusion( next_lower_bound ) );
        }
    }

    return gap;
}

FMoveResult
GetMoveInfo( TArray< UMovieSceneSection* > iSections, const UMovieSceneSection* iSection )
{
    FMoveResult move_result;

    // Get reference (iSection) stuff
    move_result.mReferenceSectionRange = iSection->GetTrueRange();
    TArray<TRange<FFrameNumber>> ranges = move_result.mReferenceSectionRange.Split( ( move_result.mReferenceSectionRange.GetLowerBoundValue().Value + move_result.mReferenceSectionRange.GetUpperBoundValue().Value ) / 2 );
    if( ranges.Num() != 2 )
        move_result.mReferenceFrame = move_result.mReferenceSectionRange.GetLowerBoundValue();
    else
        move_result.mReferenceFrame = ranges[0].GetUpperBoundValue();

    // Get all sections without the current one
    TArray<UMovieSceneSection*> sections_without_selected;
    for( int i = 0; i < iSections.Num(); i++ )
    {
        if( iSections[i] == iSection )
            continue;

        sections_without_selected.Add( iSections[i] );
    }

    //---

    if( !sections_without_selected.Num() )
        return move_result; // Nothing to do

    //---

    // Compute gap stuff
    bool is_before_gap = true;
    for( int i = 0; i < sections_without_selected.Num(); i++ )
    {
        UMovieSceneSection* current_section = sections_without_selected[i];
        TRangeBound<FFrameNumber> current_upper_bound = current_section->GetTrueRange().GetUpperBound();

        if( is_before_gap )
            move_result.mSectionsBeforeGap.Add( current_section );
        else
            move_result.mSectionsAfterGap.Add( current_section );

        if( !sections_without_selected.IsValidIndex( i + 1 ) )
            continue;

        UMovieSceneSection* next_section = sections_without_selected[i + 1];
        TRangeBound<FFrameNumber> next_lower_bound = next_section->GetTrueRange().GetLowerBound();
        if( current_upper_bound.GetValue() != next_lower_bound.GetValue() )
        {
            move_result.mInitialGap = TRange<FFrameNumber>( TRangeBound<FFrameNumber>::FlipInclusion( current_upper_bound ), TRangeBound<FFrameNumber>::FlipInclusion( next_lower_bound ) );
            is_before_gap = false;
        }
    }

    if( move_result.mInitialGap.IsEmpty() )
    {
        if( MovieSceneHelpersShift::RangeSize( TRange<FFrameNumber>( 0, sections_without_selected[0]->GetTrueRange().GetLowerBoundValue() ) ) < MovieSceneHelpersShift::RangeSize( iSection->GetTrueRange() ) ) //PATCH: to guess if the section was the first or the last
        {
            FFrameNumber section_n_upper = iSection->GetTrueRange().GetUpperBound().GetValue();
            move_result.mInitialGap = TRange<FFrameNumber>( section_n_upper, section_n_upper + MovieSceneHelpersShift::RangeSize( iSection->GetTrueRange() ) );

            move_result.mSectionsBeforeGap = sections_without_selected;
        }
        else
        {
            FFrameNumber section_0_lower = iSection->GetTrueRange().GetLowerBound().GetValue();
            move_result.mInitialGap = TRange<FFrameNumber>( section_0_lower - MovieSceneHelpersShift::RangeSize( iSection->GetTrueRange() ), section_0_lower );

            move_result.mSectionsAfterGap = sections_without_selected;
        }

        //if( &ioSection == Sections[0] || ( Sections.Num() >= 2 && &ioSection == Sections[1] ) )
        //{
        //    FFrameNumber section_0_lower = ioSection.GetTrueRange().GetLowerBound().GetValue();
        //    initial_gap = TRange<FFrameNumber>( section_0_lower - MovieSceneHelpersShift::RangeSize( ioSection.GetTrueRange() ), section_0_lower );

        //    sections_after_gap = sections_without_selected;
        //}
        //else if( &ioSection == Sections.Last() )
        //{
        //    FFrameNumber section_n_upper = ioSection.GetTrueRange().GetUpperBound().GetValue();
        //    initial_gap = TRange<FFrameNumber>( section_n_upper, section_n_upper + MovieSceneHelpersShift::RangeSize( ioSection.GetTrueRange() ) );

        //    sections_before_gap = sections_without_selected;
        //}
        //else
        //{
        //    check( false );
        //}
    }

    move_result.mInitialGapMiddle = ( move_result.mInitialGap.GetLowerBoundValue().Value + move_result.mInitialGap.GetUpperBoundValue().Value ) / 2;
    move_result.mInitialGapSize = MovieSceneHelpersShift::RangeSize( move_result.mInitialGap );
    //check( move_result.mInitialGapSize == MovieSceneHelpersShift::RangeSize( iSection->GetTrueRange() ) );

    return move_result;
}

void
FixMoveSections( TArray< UMovieSceneSection* > ioSections, UMovieSceneSection* iSection, FMoveResult iMoveResult )
{
    if( iMoveResult.mInitialGap.IsEmpty() )
        return; // Nothing to do

    for( auto current_section : ioSections )
    {
        if( current_section == iSection )
            continue;

        TRange<FFrameNumber> current_section_range( current_section->GetTrueRange() );

        if( iMoveResult.mReferenceFrame > iMoveResult.mInitialGapMiddle )
        {
            TArray<TRange<FFrameNumber>> ranges = current_section_range.Split( ( current_section_range.GetLowerBoundValue().Value + current_section_range.GetUpperBoundValue().Value ) / 2 );
            if( ranges[0].Contains( iMoveResult.mReferenceFrame ) )
            {
                for( int j = 0; j < iMoveResult.mSectionsAfterGap.Num(); j++ )
                {
                    UMovieSceneSection* current_section_to_shift = iMoveResult.mSectionsAfterGap[j];
                    if( current_section_to_shift == current_section )
                        break;

                    current_section_to_shift->MoveSection( -iMoveResult.mInitialGapSize );
                }
            }
            else if( ranges.Num() >= 2 && ranges[1].Contains( iMoveResult.mReferenceFrame ) )
            {
                for( int j = 0; j < iMoveResult.mSectionsAfterGap.Num(); j++ )
                {
                    UMovieSceneSection* current_section_to_shift = iMoveResult.mSectionsAfterGap[j];
                    current_section_to_shift->MoveSection( -iMoveResult.mInitialGapSize );

                    if( current_section_to_shift == current_section )
                        break;
                }
            }
        }
        else if( iMoveResult.mReferenceFrame < iMoveResult.mInitialGapMiddle )
        {
            TArray<TRange<FFrameNumber>> ranges = current_section_range.Split( ( current_section_range.GetLowerBoundValue().Value + current_section_range.GetUpperBoundValue().Value ) / 2 );
            if( ranges[0].Contains( iMoveResult.mReferenceFrame ) )
            {
                for( int j = iMoveResult.mSectionsBeforeGap.Num() - 1; j >= 0; j-- )
                {
                    UMovieSceneSection* current_section_to_shift = iMoveResult.mSectionsBeforeGap[j];
                    current_section_to_shift->MoveSection( iMoveResult.mInitialGapSize );

                    if( current_section_to_shift == current_section )
                        break;
                }
            }
            else if( ranges.Num() >= 2 && ranges[1].Contains( iMoveResult.mReferenceFrame ) )
            {
                for( int j = iMoveResult.mSectionsBeforeGap.Num() - 1; j >= 0; j-- )
                {
                    UMovieSceneSection* current_section_to_shift = iMoveResult.mSectionsBeforeGap[j];
                    if( current_section_to_shift == current_section )
                        break;

                    current_section_to_shift->MoveSection( iMoveResult.mInitialGapSize );
                }
            }
        }
    }

    MovieSceneHelpers::SortConsecutiveSections( ioSections );
}

#if WITH_EDITOR
void
UMovieSceneCinematicBoardTrack::OnSectionMoved( UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &ioSection );
    if( !board_section )
        return;

    //TODO: check if we need to check iParams::Interactive ?

    if( board_section->IsResizing() )
    {
        MovieSceneHelpers::FixupConsecutiveSections( Sections, ioSection, false );

        if( iParams.MoveType == EPropertyChangeType::ValueSet )
        {
            board_section->StopResizing();
        }
    }
    else // Everything else is considered as Moving
    {
        FMoveResult move_result = GetMoveInfo( Sections, &ioSection );
        FixMoveSections( Sections, &ioSection, move_result );

        if( iParams.MoveType == EPropertyChangeType::ValueSet )
        {
            // Get all sections without the current one
            TArray<UMovieSceneSection*> sections_without_selected;
            for( int i = 0; i < Sections.Num(); i++ )
            {
                if( Sections[i] == &ioSection )
                    continue;

                sections_without_selected.Add( Sections[i] );
            }

            if( !sections_without_selected.Num() )
                return;

            TRange<FFrameNumber> new_gap = FindGap( sections_without_selected );
            if( new_gap.IsEmpty() )
            {
                if( ioSection.GetTrueRange().GetLowerBoundValue() <= ( sections_without_selected[0]->GetTrueRange().GetLowerBoundValue() + sections_without_selected.Last()->GetTrueRange().GetUpperBoundValue() ) / 2 )
                {
                    ioSection.MoveSection( sections_without_selected[0]->GetTrueRange().GetLowerBoundValue() - ioSection.GetTrueRange().GetLowerBoundValue() );
                    ioSection.MoveSection( -MovieSceneHelpersShift::RangeSize( ioSection.GetTrueRange() ) );
                }
                else
                {
                    ioSection.MoveSection( sections_without_selected.Last()->GetTrueRange().GetUpperBoundValue() - ioSection.GetTrueRange().GetUpperBoundValue() );
                    ioSection.MoveSection( MovieSceneHelpersShift::RangeSize( ioSection.GetTrueRange() ) );
                }
            }
            else
            {
                ioSection.SetRange( new_gap );
            }

            MovieSceneHelpers::SortConsecutiveSections( Sections );
        }
    }
}
#endif

#if WITH_EDITORONLY_DATA
FText
UMovieSceneCinematicBoardTrack::GetDefaultDisplayName() const
{
    return LOCTEXT( "TrackName", "Boards" );
}
#endif

void
UMovieSceneCinematicBoardTrack::SortSections()
{
    MovieSceneHelpers::SortConsecutiveSections( Sections );
}

#undef LOCTEXT_NAMESPACE
