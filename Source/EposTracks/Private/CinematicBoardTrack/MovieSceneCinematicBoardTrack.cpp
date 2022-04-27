// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"

#include "MovieSceneSequence.h"
#include "MovieSceneCommonHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "Compilation/MovieSceneCompilerRules.h"

#include "ArrangeSectionsType.h"
#include "EposMovieSceneSequence.h"
#include "EposTracksModule.h"
#include "Helpers/SectionsHelpersMove.h"
#include "Helpers/SectionsHelpersResize.h"
#include "Helpers/SectionsHelpersShift.h"
#include "Settings/EposTracksSettings.h"


#define LOCTEXT_NAMESPACE "MovieSceneCinematicBoardTrack"


/* UMovieSceneSubTrack interface
 *****************************************************************************/
UMovieSceneCinematicBoardTrack::UMovieSceneCinematicBoardTrack( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 240, 100, 153, 64 );
#endif

    mArrangeSections = EArrangeSections::OnTwoRowsShifted;

    //TODO: maybe set this only moving ?
    SupportedBlendTypes.Add( EMovieSceneBlendType::Absolute ); // Only to be able to move section through other ones
}

void
UMovieSceneCinematicBoardTrack::PostInitProperties()
{
    Super::PostInitProperties();

    const UEposTracksSettings* settings = GetDefault<UEposTracksSettings>();
    mArrangeSections = settings->BoardTrackSettings.ArrangeSections;
}

UMovieSceneSubSection*
UMovieSceneCinematicBoardTrack::AddSequence( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration )
{
    return AddSequenceOnRow( iSequence, iStartTime, iDuration, INDEX_NONE );
}

UMovieSceneSubSection*
UMovieSceneCinematicBoardTrack::AddSequenceOnRow( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration, int32 iRowIndex )
{
    FShiftResult shift_result = SectionsHelpersShift::GetShiftInfo( Sections, iSequence, iStartTime, iDuration > 0 ? iDuration : TOptional<int32>() );

    //---

    UMovieSceneSubSection* newSection = UMovieSceneSubTrack::AddSequenceOnRow( iSequence, shift_result.mNewRange.GetLowerBoundValue(), UE::MovieScene::DiscreteSize( shift_result.mNewRange ), iRowIndex );

    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( newSection );
    check( board_section );
    FBoardSectionTake take( board_section->GetSequence() );
    board_section->AddTake( take );

    // When a new sequence is added, sort all sequences to ensure they are in the correct order
    SortSections();
    // Once sequences are sorted fixup the surrounding sequences to fix any gaps
    SectionsHelpersShift::ShiftFollowingSections( Sections, newSection, shift_result );
    // Should be done again as after the first one, at least 2 sections (new one and the one at this place) have the same start
    SortSections();
    // Force arranging sections
    ArrangeSections();

    UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
    check( outer_sequence );
    outer_sequence->SectionAddedOrRemoved( newSection );

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

    SortSections();
    OrganizeSections();
    ArrangeSections();

    UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
    check( outer_sequence );
    outer_sequence->SectionAddedOrRemoved( nullptr );

    // @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?
}

void
UMovieSceneCinematicBoardTrack::RemoveSectionAt( int32 iSectionIndex )
{
    UMovieSceneSection* deleted_section = Sections[iSectionIndex];

    Sections.RemoveAt( iSectionIndex );

    SortSections();
    OrganizeSections();
    ArrangeSections();

    UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
    check( outer_sequence );
    outer_sequence->SectionAddedOrRemoved( nullptr );
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
EMovieSceneSectionMovedResult
UMovieSceneCinematicBoardTrack::OnSectionMoved( UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams )
{
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &ioSection );
    if( !board_section )
        return EMovieSceneSectionMovedResult::None;

    if( board_section->IsResizing() )
    {
        UMovieSceneSection* previous_section = SectionsHelpersResize::FixupConsecutiveSections( Sections, &ioSection );

        if( iParams.MoveType == EPropertyChangeType::ValueSet )
        {
            UEposMovieSceneSequence* outer_sequence = GetTypedOuter<UEposMovieSceneSequence>();
            check( outer_sequence );
            outer_sequence->SectionResized( board_section->IsResizingLeading() ? previous_section : board_section );
            ArrangeSections();

            board_section->StopResizing();

            return EMovieSceneSectionMovedResult::SectionsChanged;
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
            TRange<FFrameNumber>& previous_range = mPreviousMove.FindChecked( board_section );
            TRange<FFrameNumber>& last_gap = mLastGapMove.FindChecked( board_section );

            FMoveResult move_result = SectionsHelpersMove::GetMoveInfo( Sections, previous_range, last_gap, board_section );
            SectionsHelpersMove::FixMoveSections( Sections, &last_gap, &ioSection, move_result );

            if( iParams.MoveType == EPropertyChangeType::ValueSet )
            {
                previous_range = mPreviousMove.FindChecked( board_section );
                last_gap = mLastGapMove.FindChecked( board_section );
                int32& cache_priority = mCacheOverlapPriority.FindChecked( board_section );

                move_result = SectionsHelpersMove::GetMoveInfo( Sections, previous_range, last_gap, board_section );
                SectionsHelpersMove::FixPostMoveSections( Sections, last_gap, &ioSection, move_result );
                ArrangeSections();

                board_section->SetOverlapPriority( cache_priority );

                board_section->StopMoving();

                mPreviousMove.Remove( board_section );
                mLastGapMove.Remove( board_section );
                mCacheOverlapPriority.Remove( board_section );

                return EMovieSceneSectionMovedResult::SectionsChanged;
            }
            else
            {
                board_section->Moving();

                mPreviousMove.FindOrAdd( board_section ) = board_section->GetTrueRange();
            }
        }
    }

    return EMovieSceneSectionMovedResult::None;
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

void
UMovieSceneCinematicBoardTrack::OrganizeSections()
{
    UMovieSceneSection* first_section = nullptr;
    if (Sections.Num())
    {
        first_section = Sections[0];
        first_section->MoveSection(-first_section->GetInclusiveStartFrame());
    }

    UMovieSceneSection* previous_section = nullptr;
    for (auto section : Sections)
    {
        if (section == first_section)
        {
            previous_section = section;
            continue;
        }

        FFrameNumber offset = section->GetInclusiveStartFrame() - previous_section->GetExclusiveEndFrame();

        section->MoveSection(-offset);

        //---

        previous_section = section;
    }
}

void
UMovieSceneCinematicBoardTrack::ArrangeSections()
{
    if( !Sections.Num() )
        return;

    if( mArrangeSections == EArrangeSections::Manually )
    {
    }
    else if( mArrangeSections == EArrangeSections::OnOneRow )
    {
        for( auto section : Sections )
        {
            section->Modify();
            section->SetRowIndex( 0 );
        }
    }
    else if( mArrangeSections == EArrangeSections::OnTwoRowsShifted )
    {
        int start = Sections[0]->GetRowIndex();
        for( int i = 0; i < Sections.Num(); i++ )
        {
            auto section = Sections[i];
            section->Modify();

            section->SetRowIndex( ( start + i ) % 2 );
        }
    }
}

void
UMovieSceneCinematicBoardTrack::SetArrangeSections( EArrangeSections iArrangeSections )
{
    mArrangeSections = iArrangeSections;

    ArrangeSections();
}

EArrangeSections
UMovieSceneCinematicBoardTrack::GetArrangeSections()
{
    return mArrangeSections;
}

#undef LOCTEXT_NAMESPACE
