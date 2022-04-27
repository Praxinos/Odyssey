// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
