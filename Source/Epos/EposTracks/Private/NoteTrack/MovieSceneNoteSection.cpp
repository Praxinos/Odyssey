// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "NoteTrack/MovieSceneNoteSection.h"

#include "UObject/SequencerObjectVersion.h"
#include "Channels/MovieSceneChannelProxy.h"

#define LOCTEXT_NAMESPACE "MovieSceneNoteSection"

//---

UMovieSceneNoteSection::UMovieSceneNoteSection( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    BlendType = EMovieSceneBlendType::Absolute;
}

void
UMovieSceneNoteSection::SetNote( UStoryNote* iNote )
{
    Note = iNote;
}

UStoryNote*
UMovieSceneNoteSection::GetNote() const
{
    return Note;
}

#undef LOCTEXT_NAMESPACE
