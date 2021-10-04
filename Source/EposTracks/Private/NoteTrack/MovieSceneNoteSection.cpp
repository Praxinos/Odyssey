// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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

#undef LOCTEXT_NAMESPACE
