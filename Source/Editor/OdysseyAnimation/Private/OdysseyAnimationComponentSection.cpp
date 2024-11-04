// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationComponentSection.h"
#include "Channels/MovieSceneChannelData.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/MovieSceneEntityBuilder.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "EntitySystem/MovieSceneEntitySystemLinker.h"
#include "EntitySystem/MovieSceneInstanceRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponentSection)

#define LOCTEXT_NAMESPACE "UOdysseyAnimationComponentSection"

UOdysseyAnimationComponentSection::UOdysseyAnimationComponentSection(const FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer)
{
}


void
UOdysseyAnimationComponentSection::PostInitProperties()
{
	Super::PostInitProperties();
}

EMovieSceneChannelProxyType
UOdysseyAnimationComponentSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData Channels;
	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
	return EMovieSceneChannelProxyType::Dynamic;
}

void
UOdysseyAnimationComponentSection::MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate)
{
	if (StartFrameOffset.Value > 0)
	{
		FFrameNumber NewStartFrameOffset = ConvertFrameTime(FFrameTime(StartFrameOffset), SourceRate, DestinationRate).FloorToFrame();
		StartFrameOffset = NewStartFrameOffset;
	}
}

#undef LOCTEXT_NAMESPACE

