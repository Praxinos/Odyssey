// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"

#include "MovieSceneSequence.h"
#include "MovieSceneCommonHelpers.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "Compilation/MovieSceneCompilerRules.h"

#include "ArrangeSectionsType.h"
#include "EposTracksModule.h"
#include "Helpers/SectionsHelpersShift.h"
#include "Helpers/SectionsHelpersMove.h"


#define LOCTEXT_NAMESPACE "MovieSceneCinematicBoardTrack"


/* UMovieSceneSubTrack interface
 *****************************************************************************/
UMovieSceneCinematicBoardTrack::UMovieSceneCinematicBoardTrack( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 0, 0, 0, 127 );
#endif

    //TODO: maybe set this only moving ?
    SupportedBlendTypes.Add( EMovieSceneBlendType::Absolute ); // Only to be able to move section through other ones
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

    FEposTracksModule::GetTracksCustomizationManager().ExecuteArrangeSections();

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

    FEposTracksModule::GetTracksCustomizationManager().ExecuteArrangeSections();

    // @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?  
}

void
UMovieSceneCinematicBoardTrack::RemoveSectionAt( int32 iSectionIndex )
{
    UMovieSceneSection* deleted_section = Sections[iSectionIndex];

    Sections.RemoveAt( iSectionIndex );
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    MovieSceneHelpersShift::ShiftFollowingSectionsAfterDelete( Sections, deleted_section ); // maybe deleted_section is not need, just remove all the gap ?

    FEposTracksModule::GetTracksCustomizationManager().ExecuteArrangeSections();
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

#if WITH_EDITOR
void
UMovieSceneCinematicBoardTrack::OnSectionMoved( UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &ioSection );
    if( !board_section )
        return;

    if( board_section->IsResizing() )
    {
        MovieSceneHelpers::FixupConsecutiveSections( Sections, ioSection, false );

        if( iParams.MoveType == EPropertyChangeType::ValueSet )
        {
            board_section->StopResizing();

            FEposTracksModule::GetTracksCustomizationManager().ExecuteArrangeSections();
        }
    }
    else
    {
        TRange<FFrameNumber> range_backup;
        if( board_section->GuessStartMoving( range_backup ) ) //TODO: certainly add all this stuff inside CinematiqueBoardSection.h/cpp once there will be a BeginMoveSection()/MoveSection()
        {
            mPreviousMove.FindOrAdd( board_section ) = range_backup;
            mLastGapMove.FindOrAdd( board_section ) = range_backup;
            mCacheOverlapPriority.FindOrAdd( board_section ) = board_section->GetOverlapPriority();

            board_section->SetOverlapPriority( 1337 /* totally arbitrary */ );

            board_section->Moving();
        }

        if( board_section->IsMoving() )
        {
            TRange<FFrameNumber>* previous_range = mPreviousMove.Find( board_section );
            check( previous_range );
            TRange<FFrameNumber>* last_gap = mLastGapMove.Find( board_section );
            check( last_gap );

            FMoveResult move_result = MovieSceneHelpersMove::GetMoveInfo( Sections, *previous_range, *last_gap, board_section );
            MovieSceneHelpersMove::FixMoveSections( Sections, last_gap, &ioSection, move_result );

            if( iParams.MoveType == EPropertyChangeType::ValueSet )
            {
                previous_range = mPreviousMove.Find( board_section );
                check( previous_range );
                last_gap = mLastGapMove.Find( board_section );
                check( last_gap );
                int32* cache_priority = mCacheOverlapPriority.Find( board_section );
                check( cache_priority );

                move_result = MovieSceneHelpersMove::GetMoveInfo( Sections, *previous_range, *last_gap, board_section );
                MovieSceneHelpersMove::FixPostMoveSections( Sections, *last_gap, &ioSection, move_result );

                board_section->SetOverlapPriority( *cache_priority );

                board_section->StopMoving();

                mPreviousMove.Remove( board_section );
                mLastGapMove.Remove( board_section );
                mCacheOverlapPriority.Remove( board_section );

                UpdateEasing();
                //TODO: find a way to call Sequencer.NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::TrackValueChanged); to be clean ?
                // as FixPostMoveSections() may move the current section and OnEndDrag() won't call it (=Notify())

                FEposTracksModule::GetTracksCustomizationManager().ExecuteArrangeSections();
            }
            else
            {
                board_section->Moving();

                mPreviousMove.FindOrAdd( board_section ) = board_section->GetTrueRange();
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
