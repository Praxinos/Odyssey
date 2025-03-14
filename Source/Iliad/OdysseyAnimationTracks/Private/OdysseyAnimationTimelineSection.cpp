// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationTimelineSection.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationComponent.h"
#include "MovieScene.h"

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
    FFrameNumber NewStartFrameOffset = ConvertFrameTime(FFrameTime(StartFrameOffset), SourceRate, DestinationRate).FloorToFrame();
    StartFrameOffset = NewStartFrameOffset;
}

TRange<FFrameNumber>
UOdysseyAnimationTimelineSection::GetDefaultSectionRange(UOdysseyAnimationTimelineSection* iSection)
{
    UOdysseyAnimation* animation = iSection->Animation;
    if (!animation)
        return TRange<FFrameNumber>();

    FFrameRate animationFrameRate(animation->GetFramesPerSecond() * 100, 100);

    FFrameNumber animationLeftBoundFrame(animation->GetLeftBoundValue());
    FFrameNumber animationRightBoundFrame(animation->GetRightBoundValue() + 1);

    UMovieScene* outer_movie_scene = iSection->GetTypedOuter<UMovieScene>();
    FFrameTime animationLeftBoundTime = FFrameRate::TransformTime(animationLeftBoundFrame, animationFrameRate, outer_movie_scene->GetDisplayRate());
    animationLeftBoundFrame = FFrameRate::TransformTime(animationLeftBoundTime, outer_movie_scene->GetDisplayRate(), outer_movie_scene->GetTickResolution()).GetFrame();
    FFrameTime animationRightBoundTime = FFrameRate::TransformTime(animationRightBoundFrame, animationFrameRate, outer_movie_scene->GetDisplayRate());
    animationRightBoundFrame = FFrameRate::TransformTime(animationRightBoundTime, outer_movie_scene->GetDisplayRate(), outer_movie_scene->GetTickResolution()).GetFrame();

    return TRange<FFrameNumber>(animationLeftBoundFrame, animationRightBoundFrame);
}

#undef LOCTEXT_NAMESPACE
