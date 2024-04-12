// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMovieRendererInterface.h"

class UEposMovieSceneSequence;
class ULevelSequence;
class UMoviePipelineMasterConfig;

class EposSequenceRenderHelpers
{
public:
    static ULevelSequence* CreateLevelSequenceTransient( TArray<UEposMovieSceneSequence*> iSequences );

    /**
     * Renders the given board sequence.
     *
     * @param iSequences The epos sequences to render (via right-click action)
     * @param iMasterConfig The configuration to use to set the renderer
     */
    //static void RenderMovie( TArray<UEposMovieSceneSequence*> iSequences, UMoviePipelineMasterConfig* iMasterConfig );

};
