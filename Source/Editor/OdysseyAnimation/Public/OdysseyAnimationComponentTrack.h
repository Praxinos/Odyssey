// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tracks/MovieSceneSubTrack.h"
#include "Compilation/IMovieSceneTrackTemplateProducer.h"
#include "OdysseyAnimationComponentTrack.generated.h"

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationComponentTrack
	: public UMovieSceneSubTrack
	, public IMovieSceneTrackTemplateProducer
{
public:
	GENERATED_BODY()

	UOdysseyAnimationComponentTrack(const FObjectInitializer& ObjectInitializer);

	virtual UMovieSceneSection* AddNewSection(FFrameNumber KeyTime, float iDurationInSeconds);

	// UMovieSceneTrack interface
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual EMovieSceneTrackEasingSupportFlags SupportsEasing(FMovieSceneSupportsEasingParams& Params) const;
	virtual bool SupportsMultipleRows() const override;
	virtual UMovieSceneSection* CreateNewSection() override;
	virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif

public:
	UPROPERTY()
	bool DisplayLayers = true;
};