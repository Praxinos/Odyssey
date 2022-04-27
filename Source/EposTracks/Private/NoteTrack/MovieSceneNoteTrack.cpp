// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "NoteTrack/MovieSceneNoteTrack.h"

#include "MovieScene.h"
#include "Evaluation/MovieSceneSegment.h"
#include "Compilation/MovieSceneSegmentCompiler.h"
#include "MovieSceneCommonHelpers.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "MovieSceneNoteTrack"

//---

UMovieSceneNoteTrack::UMovieSceneNoteTrack( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    SupportedBlendTypes.Add( EMovieSceneBlendType::Absolute );
#if WITH_EDITORONLY_DATA
    TrackTint = FColor( 90, 90, 150 ); // Also used inside style
    RowHeight = 50;
#endif
}

const TArray<UMovieSceneSection*>&
UMovieSceneNoteTrack::GetAllSections() const //override
{
    return NoteSections;
}

bool
UMovieSceneNoteTrack::SupportsMultipleRows() const //override
{
    return true;
}

bool
UMovieSceneNoteTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const //override
{
    return SectionClass == UMovieSceneNoteSection::StaticClass();
}

void
UMovieSceneNoteTrack::RemoveAllAnimationData() //override
{
    NoteSections.Empty();
}

bool
UMovieSceneNoteTrack::HasSection( const UMovieSceneSection& Section ) const //override
{
    return NoteSections.Contains( &Section );
}

void
UMovieSceneNoteTrack::AddSection( UMovieSceneSection& Section ) //override
{
    NoteSections.Add( &Section );
}

void
UMovieSceneNoteTrack::RemoveSection( UMovieSceneSection& Section ) //override
{
    NoteSections.Remove( &Section );
}

void
UMovieSceneNoteTrack::RemoveSectionAt( int32 SectionIndex ) //override
{
    NoteSections.RemoveAt( SectionIndex );
}

bool
UMovieSceneNoteTrack::IsEmpty() const //override
{
    return NoteSections.Num() == 0;
}

UMovieSceneSection*
UMovieSceneNoteTrack::CreateNewSection() //override
{
    return NewObject<UMovieSceneNoteSection>( this, NAME_None, RF_Transactional );
}

#if WITH_EDITORONLY_DATA
FText
UMovieSceneNoteTrack::GetDefaultDisplayName() const
{
    return LOCTEXT( "TrackName", "Note" );
}
#endif

//---

const TArray<UMovieSceneSection*>&
UMovieSceneNoteTrack::GetNoteSections() const
{
    return NoteSections;
}

UMovieSceneSection*
UMovieSceneNoteTrack::AddNewNote( UStoryNote* iNote, FFrameNumber iStartTime, int32 iDuration )
{
    return AddNewNoteOnRow( iNote, iStartTime, iDuration, INDEX_NONE );
}

UMovieSceneSection*
UMovieSceneNoteTrack::AddNewNoteOnRow( UStoryNote* iNote, FFrameNumber iStartTime, int32 iDuration, int32 RowIndex )
{
    // add the section
    UMovieSceneNoteSection* NewSection = NewObject<UMovieSceneNoteSection>( this, NAME_None, RF_Transactional );
    NewSection->InitialPlacementOnRow( NoteSections, iStartTime, iDuration, RowIndex );
    NewSection->SetNote( iNote );

    NoteSections.Add( NewSection );

    return NewSection;
}

bool
UMovieSceneNoteTrack::IsAMasterTrack() const
{
    UMovieScene* MovieScene = Cast<UMovieScene>( GetOuter() );
    return MovieScene ? MovieScene->IsAMasterTrack( *this ) : false;
}

//---

#if WITH_EDITORONLY_DATA

int32
UMovieSceneNoteTrack::GetRowHeight() const
{
    return RowHeight;
}

void
UMovieSceneNoteTrack::SetRowHeight( int32 NewRowHeight )
{
    RowHeight = FMath::Max( 16, NewRowHeight );
}

#endif

#undef LOCTEXT_NAMESPACE
