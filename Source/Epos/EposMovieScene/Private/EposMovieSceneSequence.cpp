// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposMovieSceneSequence.h"

//---

UEposMovieSceneSequence::UEposMovieSceneSequence(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //PATCH: Should be done via Engine.ini config file, but doesn't work in 5.0
    DefaultCompletionMode = EMovieSceneCompletionMode::RestoreState;
}
