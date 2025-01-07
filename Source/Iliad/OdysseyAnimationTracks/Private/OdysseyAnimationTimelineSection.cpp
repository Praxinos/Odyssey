// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationTimelineSection.h"

#include "Channels/MovieSceneChannelData.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/MovieSceneEntityBuilder.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "EntitySystem/MovieSceneEntitySystemLinker.h"
#include "EntitySystem/MovieSceneInstanceRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationTimelineSection)

#define LOCTEXT_NAMESPACE "UOdysseyAnimationTimelineSection"

UOdysseyAnimationTimelineSection::UOdysseyAnimationTimelineSection(const FObjectInitializer& ObjInitializer)
    : Super(ObjInitializer)
{
}

//---

void
UOdysseyAnimationTimelineSection::PostInitProperties()
{
    Super::PostInitProperties();
}

EMovieSceneChannelProxyType
UOdysseyAnimationTimelineSection::CacheChannelProxy()
{
    FMovieSceneChannelProxyData Channels;
    ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));
    return EMovieSceneChannelProxyType::Dynamic;
}

void
UOdysseyAnimationTimelineSection::MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate)
{
    if (StartFrameOffset.Value > 0)
    {
        FFrameNumber NewStartFrameOffset = ConvertFrameTime(FFrameTime(StartFrameOffset), SourceRate, DestinationRate).FloorToFrame();
        StartFrameOffset = NewStartFrameOffset;
    }
}

#undef LOCTEXT_NAMESPACE
