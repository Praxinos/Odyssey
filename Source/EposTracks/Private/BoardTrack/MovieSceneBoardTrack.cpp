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
UMovieSceneBoardTrack::UMovieSceneBoardTrack( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 0, 0, 0, 127 );
#endif
}

UMovieSceneSubSection*
UMovieSceneBoardTrack::AddSequence( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration )
{
    return AddSequenceOnRow( iSequence, iStartTime, iDuration, INDEX_NONE );
}

UMovieSceneSubSection*
UMovieSceneBoardTrack::AddSequenceOnRow( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration, int32 iRowIndex )
{
    UMovieSceneSubSection* newSection = UMovieSceneSubTrack::AddSequenceOnRow( iSequence, iStartTime, iDuration, iRowIndex );

    UMovieSceneBoardSection* newBoardSection = Cast<UMovieSceneBoardSection>( newSection );

#if WITH_EDITOR

    if( iSequence != nullptr )
    {
        newBoardSection->SetBoardDisplayName( iSequence->GetDisplayName().ToString() );
    }

#endif

    // When a new sequence is added, sort all sequences to ensure they are in the correct order
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    // Once sequences are sorted fixup the surrounding sequences to fix any gaps
    //MovieSceneHelpers::FixupConsecutiveSections(Sections, *NewSection, false);

    return newSection;
}

/* UMovieSceneTrack interface
 *****************************************************************************/

void
UMovieSceneBoardTrack::AddSection( UMovieSceneSection& ioSection )
{
    if( ioSection.IsA<UMovieSceneBoardSection>() )
    {
        Sections.Add( &ioSection );
    }
}

bool
UMovieSceneBoardTrack::SupportsType( TSubclassOf<UMovieSceneSection> iSectionClass ) const
{
    return iSectionClass == UMovieSceneBoardSection::StaticClass();
}


UMovieSceneSection*
UMovieSceneBoardTrack::CreateNewSection()
{
    return NewObject<UMovieSceneBoardSection>( this, NAME_None, RF_Transactional );
}

void
UMovieSceneBoardTrack::RemoveSection( UMovieSceneSection& ioSection )
{
    Sections.Remove( &ioSection );
    //MovieSceneHelpers::FixupConsecutiveSections(Sections, Section, true);
    MovieSceneHelpers::SortConsecutiveSections( Sections );

    // @todo Sequencer: The movie scene owned by the section is now abandoned.  Should we offer to delete it?  
}

void
UMovieSceneBoardTrack::RemoveSectionAt( int32 iSectionIndex )
{
    Sections.RemoveAt( iSectionIndex );
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
        virtual void Blend( FSegmentBlendData& ioBlendData ) const override
        {
            MovieSceneSegmentCompiler::ChooseLowestRowIndex( ioBlendData );
        }
    };
    return FBoardTrackRowBlender();
}

FMovieSceneTrackRowSegmentBlenderPtr
UMovieSceneBoardTrack::GetRowSegmentBlender() const
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

#if WITH_EDITOR
void
UMovieSceneBoardTrack::OnSectionMoved( UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams )
{
    //MovieSceneHelpers::FixupConsecutiveSections(Sections, ioSection, false);
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
