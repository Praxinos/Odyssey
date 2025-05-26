// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IMovieRendererInterface.h"

class UMovieSceneCinematicShotSection;
class UMovieSceneSequence;

class FExportSequencerRenderer
    : public IMovieRendererInterface
{
public:
    static FName ExportTabName;
    static FText ExportTabLabel;

public:
    virtual void RenderMovie( UMovieSceneSequence* InSequence, const TArray<UMovieSceneCinematicShotSection*>& InSections ) override;

    virtual FString GetDisplayName() const override;
};
