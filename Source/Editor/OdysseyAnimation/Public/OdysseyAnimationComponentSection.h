// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Sections/MovieSceneSubSection.h"
#include "OdysseyAnimationComponentSection.generated.h"

enum class EMovieSceneChannelProxyType : uint8;

/**
 * Defines the section for a template sequence track.
 */
UCLASS()
class ODYSSEYANIMATION_API UOdysseyAnimationComponentSection 
	: public UMovieSceneSubSection
{
public:

	GENERATED_BODY()

	UOdysseyAnimationComponentSection(const FObjectInitializer& ObjInitializer);

protected:
	virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
};
