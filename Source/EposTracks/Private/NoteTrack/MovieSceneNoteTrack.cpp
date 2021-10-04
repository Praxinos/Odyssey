// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "NoteTrack/MovieSceneNoteTrack.h"

#include "MovieScene.h"
#include "Evaluation/MovieSceneSegment.h"
#include "Compilation/MovieSceneSegmentCompiler.h"
#include "MovieSceneCommonHelpers.h"

#include "NoteTrack/MovieSceneNoteSection.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "MovieSceneNoteTrack"

//---

UMovieSceneNoteTrack::UMovieSceneNoteTrack( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    SupportedBlendTypes.Add( EMovieSceneBlendType::Absolute );
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 136, 95, 93 );
    RowHeight = 50;
#endif
}

const TArray<UMovieSceneSection*>& UMovieSceneNoteTrack::GetAllSections() const
{
    return NoteSections;
}

bool UMovieSceneNoteTrack::SupportsMultipleRows() const
{
    return true;
}

bool UMovieSceneNoteTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const
{
    return SectionClass == UMovieSceneNoteSection::StaticClass();
}

void UMovieSceneNoteTrack::RemoveAllAnimationData()
{
    NoteSections.Empty();
}

bool UMovieSceneNoteTrack::HasSection( const UMovieSceneSection& Section ) const
{
    return NoteSections.Contains( &Section );
}

void UMovieSceneNoteTrack::AddSection( UMovieSceneSection& Section )
{
    NoteSections.Add( &Section );
}

void UMovieSceneNoteTrack::RemoveSection( UMovieSceneSection& Section )
{
    NoteSections.Remove( &Section );
}

void UMovieSceneNoteTrack::RemoveSectionAt( int32 SectionIndex )
{
    NoteSections.RemoveAt( SectionIndex );
}

bool UMovieSceneNoteTrack::IsEmpty() const
{
    return NoteSections.Num() == 0;
}

UMovieSceneSection* UMovieSceneNoteTrack::AddNewNoteOnRow( UStoryNote* iNote, FFrameNumber Time, int32 RowIndex )
{
    FFrameRate FrameRate = GetTypedOuter<UMovieScene>()->GetTickResolution();

    // determine initial duration
    // @todo Once we have infinite sections, we can remove this
    // @todo ^^ Why? Infinte sections would mean there's no starting time?
    FFrameTime DurationToUse = 1.f * FrameRate; // if all else fails, use 1 second duration

    float NoteDuration = 5.f; //TODO
    //float SoundDuration = MovieSceneHelpers::GetSoundDuration( Sound );
    if( NoteDuration != INDEFINITELY_LOOPING_DURATION )
    {
        DurationToUse = NoteDuration * FrameRate;
    }

    // add the section
    UMovieSceneNoteSection* NewSection = NewObject<UMovieSceneNoteSection>( this, NAME_None, RF_Transactional );
    NewSection->InitialPlacementOnRow( NoteSections, Time, DurationToUse.FrameNumber.Value, RowIndex );
    NewSection->SetNote( iNote );

    NoteSections.Add( NewSection );

    return NewSection;
}

bool UMovieSceneNoteTrack::IsAMasterTrack() const
{
    UMovieScene* MovieScene = Cast<UMovieScene>( GetOuter() );
    return MovieScene ? MovieScene->IsAMasterTrack( *this ) : false;
}

UMovieSceneSection* UMovieSceneNoteTrack::CreateNewSection()
{
    return NewObject<UMovieSceneNoteSection>( this, NAME_None, RF_Transactional );
}

#undef LOCTEXT_NAMESPACE
