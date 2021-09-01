// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UEposMovieSceneSequence;

class EposSequenceRenderHelpers
{
public:
    /**
     * Renders the given board sequence.
     *
     * @param iSequences The epos sequences to render
     */
    static void RenderMovie( TArray<UEposMovieSceneSequence*> iSequences );

};
