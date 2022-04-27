// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMovieRendererInterface.h"

class UMovieSceneCinematicShotSection;
class UMovieSceneSequence;
class UMoviePipelineMasterConfig;

class FEposSequencePipelineRenderer
    : public IMovieRendererInterface
{
public:
    static FName MoviePipelineQueueTabName;
    static FText MoviePipelineQueueTabLabel;

public:
    virtual void RenderMovie( UMovieSceneSequence* InSequence, const TArray<UMovieSceneCinematicShotSection*>& InSections ) override;

    virtual FString GetDisplayName() const override;

public:
    UMoviePipelineMasterConfig* mMasterConfig { nullptr };
};
