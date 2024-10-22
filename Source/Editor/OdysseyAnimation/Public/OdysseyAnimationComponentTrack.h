// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tracks/MovieSceneSubTrack.h"
#include "OdysseyAnimationComponentTrack.generated.h"

UCLASS(MinimalAPI)
class UOdysseyAnimationComponentTrack
	: public UMovieSceneSubTrack
{
public:
	GENERATED_BODY()

	UOdysseyAnimationComponentTrack(const FObjectInitializer& ObjectInitializer);

	virtual UMovieSceneSection* AddNewSection(FFrameNumber KeyTime, UOdysseyAnimationComponent* iComponent);

	// UMovieSceneTrack interface
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual UMovieSceneSection* CreateNewSection() override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif
};