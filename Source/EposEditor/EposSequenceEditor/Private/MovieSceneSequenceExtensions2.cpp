// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
