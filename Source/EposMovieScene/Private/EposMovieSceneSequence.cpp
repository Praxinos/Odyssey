// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposMovieSceneSequence.h"

//---

UEposMovieSceneSequence::UEposMovieSceneSequence(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //PATCH: Should be done via Engine.ini config file, but doesn't work in 5.0
    DefaultCompletionMode = EMovieSceneCompletionMode::RestoreState;
}
