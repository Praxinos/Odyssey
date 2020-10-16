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

    UMovieSceneSubSection* newSection = UMovieSceneSubTrack::AddSequenceOnRow( iSequence, shift_result.mNewRange.GetLowerBoundValue(), shift_result.mNewRange.Size<FFrameNumber>().Value, iRowIndex );

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

FMoveResult
GetMoveInfo( TArray< UMovieSceneSection* > iSections, TMap<UMovieSceneSection*, TRange<FFrameNumber>> iPreviousMove, TMap<UMovieSceneSection*, TRange<FFrameNumber>> iLastGapMove, const UMovieSceneCinematicBoardSection* iSection )
{
    FMoveResult move_result;

    TRange<FFrameNumber>* previous_range = iPreviousMove.Find( iSection );

    if( iSection->GetInclusiveStartFrame() > previous_range->GetLowerBoundValue() )
        move_result.mForward = true;
    else if( iSection->GetInclusiveStartFrame() < previous_range->GetLowerBoundValue() )
        move_result.mBackward = true;
    else
        return move_result;

    if( move_result.mForward )
        move_result.mReferenceFrame = iSection->GetExclusiveEndFrame();
    else
        move_result.mReferenceFrame = iSection->GetInclusiveStartFrame();

    //---

    move_result.mInitialGap = *iLastGapMove.Find( iSection );
    move_result.mInitialGapMiddle = ( move_result.mInitialGap.GetLowerBoundValue() + move_result.mInitialGap.GetUpperBoundValue() ) / 2;
    move_result.mInitialGapSize = move_result.mInitialGap.Size<FFrameNumber>();

    //---

    for( int i = 0; i < iSections.Num(); i++ )
    {
        if( iSections[i] == iSection )
            continue;

        UMovieSceneSection* section = iSections[i];
        FFrameNumber section_middle = ( section->GetInclusiveStartFrame() + section->GetExclusiveEndFrame() ) / 2;

        if( section_middle < move_result.mInitialGapMiddle )
            move_result.mSectionsBeforeGap.Add( section );
        else
            move_result.mSectionsAfterGap.Add( section );
    }


    Algo::Reverse( move_result.mSectionsBeforeGap );

    //UE_LOG( LogTemp, Warning, TEXT( "previous range: [%d, %d) - current range: [%d, %d) - forward %d backward %d - ref frame: %d - gap: [%d, %d, %d) - sections before %d after %d" )
    //                                                                                                                                                , previous_range->GetLowerBoundValue().Value, previous_range->GetUpperBoundValue().Value, iSection->GetInclusiveStartFrame().Value, iSection->GetExclusiveEndFrame().Value
    //                                                                                                                                                , move_result.mForward, move_result.mBackward, move_result.mReferenceFrame.Value
    //                                                                                                                                                , move_result.mInitialGap.GetLowerBoundValue().Value, move_result.mInitialGap.GetUpperBoundValue().Value, move_result.mInitialGapMiddle.Value
    //                                                                                                                                                , move_result.mSectionsBeforeGap.Num(), move_result.mSectionsAfterGap.Num() );

    return move_result;
}

void
FixMoveSections( TArray< UMovieSceneSection* >& ioSections, TMap<UMovieSceneSection*, TRange<FFrameNumber>>& iLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult )
{
    // If the reference frame is at the right of the gap,
    // process only every sections 'AfterGap'
    if( iMoveResult.mReferenceFrame > iMoveResult.mInitialGapMiddle )
    {
        // Find the section (if any) corresponding to the reference frame
        UMovieSceneSection* section_containing_reference_frame = nullptr;
        TArray<TRange<FFrameNumber>> ranges;
        for( auto section : iMoveResult.mSectionsAfterGap )
        {
            if( section->IsTimeWithinSection( iMoveResult.mReferenceFrame ) )
            {
                section_containing_reference_frame = section;
                ranges = section->GetTrueRange().Split( ( section->GetInclusiveStartFrame() + section->GetExclusiveEndFrame() ) / 2 );
                break;
            }
        }

        if( ranges.Num() && ranges[0].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'after gap' sections before (and excluding) the referenced one
            for( auto section_after_gap : iMoveResult.mSectionsAfterGap )
            {
                if( section_after_gap == section_containing_reference_frame )
                    break;

                TRange<FFrameNumber>* gap = iLastGapMove.Find( iSection );
                FFrameNumber gap_shift = section_after_gap->GetTrueRange().Size<FFrameNumber>();
                *gap = TRange<FFrameNumber>( gap->GetLowerBoundValue() + gap_shift, gap->GetUpperBoundValue() + gap_shift );

                section_after_gap->MoveSection( -iMoveResult.mInitialGapSize );
            }
        }
        else if( ranges.Num() >= 2 && ranges[1].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'after gap' sections before (and including) the referenced one
            for( auto section_after_gap : iMoveResult.mSectionsAfterGap )
            {
                TRange<FFrameNumber>* gap = iLastGapMove.Find( iSection );
                FFrameNumber gap_shift = section_after_gap->GetTrueRange().Size<FFrameNumber>();
                *gap = TRange<FFrameNumber>( gap->GetLowerBoundValue() + gap_shift, gap->GetUpperBoundValue() + gap_shift );

                section_after_gap->MoveSection( -iMoveResult.mInitialGapSize );

                if( section_after_gap == section_containing_reference_frame )
                    break;
            }
        }
    }
    // If the reference frame is at the left of the gap,
    // process only every sections 'BeforeGap'
    else if( iMoveResult.mReferenceFrame < iMoveResult.mInitialGapMiddle )
    {
        // Find the section (if any) corresponding to the reference frame
        UMovieSceneSection* section_containing_reference_frame = nullptr;
        TArray<TRange<FFrameNumber>> ranges;
        for( auto section : iMoveResult.mSectionsBeforeGap )
        {
            if( section->IsTimeWithinSection( iMoveResult.mReferenceFrame ) )
            {
                section_containing_reference_frame = section;
                ranges = section->GetTrueRange().Split( ( section->GetInclusiveStartFrame() + section->GetExclusiveEndFrame() ) / 2 );
                break;
            }
        }

        if( ranges.Num() && ranges[0].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'before gap' sections after (and including) the referenced one
            for( auto section_before_gap : iMoveResult.mSectionsBeforeGap )
            {
                TRange<FFrameNumber>* gap = iLastGapMove.Find( iSection );
                FFrameNumber gap_shift = section_before_gap->GetTrueRange().Size<FFrameNumber>();
                *gap = TRange<FFrameNumber>( gap->GetLowerBoundValue() - gap_shift, gap->GetUpperBoundValue() - gap_shift );

                section_before_gap->MoveSection( iMoveResult.mInitialGapSize );

                if( section_before_gap == section_containing_reference_frame )
                    break;
            }
        }
        else if( ranges.Num() >= 2 && ranges[1].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'before gap' sections after (and excluding) the referenced one
            for( auto section_before_gap : iMoveResult.mSectionsBeforeGap )
            {
                if( section_before_gap == section_containing_reference_frame )
                    break;

                TRange<FFrameNumber>* gap = iLastGapMove.Find( iSection );
                FFrameNumber gap_shift = section_before_gap->GetTrueRange().Size<FFrameNumber>();
                *gap = TRange<FFrameNumber>( gap->GetLowerBoundValue() - gap_shift, gap->GetUpperBoundValue() - gap_shift );

                section_before_gap->MoveSection( iMoveResult.mInitialGapSize );
            }
        }
    }

    MovieSceneHelpers::SortConsecutiveSections( ioSections );
}

void
FixPostMoveSections( TArray< UMovieSceneSection* >& ioSections, TMap<UMovieSceneSection*, TRange<FFrameNumber>>& iLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult )
{
    TRange<FFrameNumber>* gap = iLastGapMove.Find( iSection );
    iSection->SetRange( *gap );

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
    else
    {
        if( board_section->GuessStartMoving() )
        {
            TRange<FFrameNumber>& previous_range = mPreviousMove.FindOrAdd( board_section );
            previous_range = board_section->GetRangeBackup();
            TRange<FFrameNumber>& last_gap = mLastGapMove.FindOrAdd( board_section );
            last_gap = board_section->GetRangeBackup();

            board_section->Moving();
        }

        if( board_section->IsMoving() )
        {
            FMoveResult move_result = GetMoveInfo( Sections, mPreviousMove, mLastGapMove, board_section );
            FixMoveSections( Sections, mLastGapMove, &ioSection, move_result );

            if( iParams.MoveType == EPropertyChangeType::ValueSet )
            {
                move_result = GetMoveInfo( Sections, mPreviousMove, mLastGapMove, board_section );
                FixPostMoveSections( Sections, mLastGapMove, &ioSection, move_result );

                board_section->StopMoving();
                mPreviousMove.Remove( board_section );
            }
            else
            {
                board_section->Moving();
                TRange<FFrameNumber>& previous_range = mPreviousMove.FindOrAdd( board_section );
                previous_range = board_section->GetTrueRange();
            }
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
