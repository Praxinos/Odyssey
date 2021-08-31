// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UEposMovieSceneSequence;

class EposSequenceRenderHelpers
{
public:
    /*
    * Creates a new LevelSequence containing the given sequences in shots
    * Shots are ordered alphabetical order according to the sequences names
    *
    * @param iSequences The Epos sequences to creates the level sequence from
    * @return
    */
    static void CreateLevelSequenceFromEposSequences( TArray<UEposMovieSceneSequence*> iSequences );

    /**
     * Renders the given board sequence.
     *
     * @param iSequence The epos sequence to render
     * @return
     */
    static void RenderSequences( TArray<UEposMovieSceneSequence*> iSequences );

};
