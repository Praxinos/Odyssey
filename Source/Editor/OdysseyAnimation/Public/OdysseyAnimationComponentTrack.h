// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Tracks/MovieSceneSubTrack.h"
#include "OdysseyAnimationComponentTrack.generated.h"

UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationComponentTrack
	: public UMovieSceneSubTrack
{
public:
	GENERATED_BODY()

	UOdysseyAnimationComponentTrack(const FObjectInitializer& ObjectInitializer);

	virtual UMovieSceneSection* AddNewSection(FFrameNumber KeyTime);

	// UMovieSceneTrack interface
	virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
	virtual UMovieSceneSection* CreateNewSection() override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif

public:
	UPROPERTY()
	UOdysseyAnimationComponent* Component;
};