// IDDN.FR.000.000000.000.S.X.0000.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#include "MovieSceneSequenceExtensions2.h"

#include "MovieSceneSequence.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MovieSceneSequenceExtensions2)

FText UMovieSceneSequenceExtensions2::GetDisplayName(const UMovieSceneSequence* Sequence)
{
    if (!Sequence)
    {
        FFrame::KismetExecutionMessage(TEXT("Cannot call GetDisplayName on a null sequence"), ELogVerbosity::Error);
        return FText::GetEmpty();
    }

    return Sequence->GetDisplayName();
}
