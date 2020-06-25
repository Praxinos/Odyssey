// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "BoardTrack/MovieSceneBoardTrack.h"

#include "MovieSceneSequence.h"
#include "MovieSceneCommonHelpers.h"
#include "BoardTrack/MovieSceneBoardSection.h"
#include "Compilation/MovieSceneCompilerRules.h"


#define LOCTEXT_NAMESPACE "MovieSceneBoardTrack"


/* UMovieSceneSubTrack interface
 *****************************************************************************/
UMovieSceneBoardTrack::UMovieSceneBoardTrack( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 0, 0, 0, 127 );
#endif
}

UMovieSceneSubSection*
UMovieSceneBoardTrack::AddSequence( UMovieSceneSequence* Sequence, FFrameNumber StartTime, int32 Duration )
{
    return AddSequenceOnRow( Sequence, StartTime, Duration, INDEX_NONE );
}

UMovieSceneSubSection*
UMovieSceneBoardTrack::AddSequenceOnRow( UMovieSceneSequence* Sequence, FFrameNumber StartTime, int32 Duration, int32 RowIndex )
{
    UMovieSceneSubSection* NewSection = UMovieSceneSubTrack::AddSequenceOnRow( Sequence, StartTime, Duration, RowIndex );

    UMovieSceneBoardSection* NewBoardSection = Cast<UMovieSceneBoardSection>( NewSection );

#if WITH_EDITOR

    if( Sequence != nullptr )
    {
        NewBoardSection->SetBoardDisplayName( Sequence->GetDisplayName().ToString() );
    }

#endif

    // When a new sequence is added, sort all sequences to ensure they are in the correct order
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    // Once sequences are sorted fixup the surrounding sequences to fix any gaps
    //MovieSceneHelpers::FixupConsecutiveSections(Sections, *NewSection, false);

    return NewSection;
}

/* UMovieSceneTrack interface
 *****************************************************************************/

void
UMovieSceneBoardTrack::AddSection( UMovieSceneSection& Section )
{
    if( Section.IsA<UMovieSceneBoardSection>() )
    {
        Sections.Add( &Section );
    }
}

bool
UMovieSceneBoardTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const
{
    return SectionClass == UMovieSceneBoardSection::StaticClass();
}


UMovieSceneSection*
UMovieSceneBoardTrack::CreateNewSection()
{
    return NewObject<UMovieSceneBoardSection>( this, NAME_None, RF_Transactional );
}

void
UMovieSceneBoardTrack::RemoveSection( UMovieSceneSection& Section )
{
    Sections.Remove( &Section );
    //MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, true);
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    // @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?  
}

void
UMovieSceneBoardTrack::RemoveSectionAt( int32 SectionIndex )
{
    Sections.RemoveAt( SectionIndex );
    MovieSceneHelpers::SortConsecutiveSections( Sections );
}

bool
UMovieSceneBoardTrack::SupportsMultipleRows() const
{
    return true;
}

FMovieSceneTrackSegmentBlenderPtr
UMovieSceneBoardTrack::GetTrackSegmentBlender() const
{
    // Apply a high pass filter to overlapping sections such that only the highest row in a track wins
    struct FBoardTrackRowBlender : FMovieSceneTrackSegmentBlender
    {
        virtual void Blend( FSegmentBlendData& BlendData ) const override
        {
            MovieSceneSegmentCompiler::ChooseLowestRowIndex( BlendData );
        }
    };
    return FBoardTrackRowBlender();
}

FMovieSceneTrackRowSegmentBlenderPtr
UMovieSceneBoardTrack::GetRowSegmentBlender() const
{
    class FCinematicRowRules : public FMovieSceneTrackRowSegmentBlender
    {
        virtual void Blend( FSegmentBlendData& BlendData ) const override
        {
            // Sort everything by priority, then latest start time wins
            if( BlendData.Num() <= 1 )
            {
                return;
            }

            BlendData.Sort( SortPredicate );

            int32 RemoveAtIndex = 0;
            // Skip over any pre/postroll sections
            while( BlendData.IsValidIndex( RemoveAtIndex ) && EnumHasAnyFlags( BlendData[RemoveAtIndex].Flags, ESectionEvaluationFlags::PreRoll | ESectionEvaluationFlags::PostRoll ) )
            {
                ++RemoveAtIndex;
            }

            // Skip over the first genuine evaluation if it exists
            ++RemoveAtIndex;

            int32 NumToRemove = BlendData.Num() - RemoveAtIndex;
            if( NumToRemove > 0 )
            {
                BlendData.RemoveAt( RemoveAtIndex, NumToRemove, true );
            }
        }

        static bool SortPredicate( const FMovieSceneSectionData& A, const FMovieSceneSectionData& B )
        {
            // Always sort pre/postroll to the front of the array
            const bool PrePostRollA = EnumHasAnyFlags( A.Flags, ESectionEvaluationFlags::PreRoll | ESectionEvaluationFlags::PostRoll );
            const bool PrePostRollB = EnumHasAnyFlags( B.Flags, ESectionEvaluationFlags::PreRoll | ESectionEvaluationFlags::PostRoll );

            if( PrePostRollA != PrePostRollB )
            {
                return PrePostRollA;
            }
            else if( PrePostRollA )
            {
                return false;
            }
            else if( A.Section->GetOverlapPriority() == B.Section->GetOverlapPriority() )
            {
                TRangeBound<FFrameNumber> StartBoundA = A.Section->GetRange().GetLowerBound();
                return TRangeBound<FFrameNumber>::MaxLower( StartBoundA, B.Section->GetRange().GetLowerBound() ) == StartBoundA;
            }
            return A.Section->GetOverlapPriority() > B.Section->GetOverlapPriority();
        }
    };

    return FCinematicRowRules();
}

#if WITH_EDITOR
void
UMovieSceneBoardTrack::OnSectionMoved( UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params )
{
    //MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, false);
}
#endif

#if WITH_EDITORONLY_DATA
FText
UMovieSceneBoardTrack::GetDefaultDisplayName() const
{
    return LOCTEXT( "TrackName", "Boards" );
}
#endif

void
UMovieSceneBoardTrack::SortSections()
{
    MovieSceneHelpers::SortConsecutiveSections( Sections );
}

#undef LOCTEXT_NAMESPACE
