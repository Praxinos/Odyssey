// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "NoteTrack/MovieSceneNoteTrack.h"

#include "MovieScene.h"
#include "Evaluation/MovieSceneSegment.h"
#include "Compilation/MovieSceneSegmentCompiler.h"
#include "MovieSceneCommonHelpers.h"

#include "NoteTrack/MovieSceneNoteSection.h"

#define LOCTEXT_NAMESPACE "MovieSceneNoteTrack"


/* UMovieScenePatchStringTrack interface
 *****************************************************************************/

UMovieScenePatchStringTrack::UMovieScenePatchStringTrack( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    SupportedBlendTypes = FMovieSceneBlendTypeField::All();
}

void UMovieScenePatchStringTrack::AddSection( UMovieSceneSection& Section )
{
    Sections.Add( &Section );
}


//bool UMovieScenePatchStringTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const
//{
//    return SectionClass == UMovieSceneStringSection::StaticClass();
//}
//
//UMovieSceneSection* UMovieScenePatchStringTrack::CreateNewSection()
//{
//    return NewObject<UMovieSceneStringSection>( this, NAME_None, RF_Transactional );
//}


const TArray<UMovieSceneSection*>& UMovieScenePatchStringTrack::GetAllSections() const
{
    return Sections;
}


bool UMovieScenePatchStringTrack::HasSection( const UMovieSceneSection& Section ) const
{
    return Sections.Contains( &Section );
}


bool UMovieScenePatchStringTrack::IsEmpty() const
{
    return ( Sections.Num() == 0 );
}


void UMovieScenePatchStringTrack::RemoveAllAnimationData()
{
    Sections.Empty();
}


void UMovieScenePatchStringTrack::RemoveSection( UMovieSceneSection& Section )
{
    Sections.Remove( &Section );
}


void UMovieScenePatchStringTrack::RemoveSectionAt( int32 SectionIndex )
{
    Sections.RemoveAt( SectionIndex );
}

//---

//UMovieSceneNoteTrack::UMovieSceneNoteTrack()
//    : Super()
UMovieSceneNoteTrack::UMovieSceneNoteTrack(const FObjectInitializer& Init)
    : Super(Init)
{
}

bool UMovieSceneNoteTrack::SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const
{
    return SectionClass == UMovieSceneNoteSection::StaticClass();
}

UMovieSceneSection* UMovieSceneNoteTrack::CreateNewSection()
{
    return NewObject<UMovieSceneNoteSection>( this, NAME_None, RF_Transactional );
}

#if WITH_EDITORONLY_DATA
FText UMovieSceneNoteTrack::GetDefaultDisplayName() const
{
    return LOCTEXT( "TrackName", "Note" );
}
#endif

#undef LOCTEXT_NAMESPACE
