// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "MovieSceneSection.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"

#include "OdysseyAnimationTimelineSection.generated.h"

enum class EMovieSceneChannelProxyType : uint8;
class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

/**
 * Defines the section for a template sequence track.
 */
UCLASS()
class ODYSSEYANIMATIONTRACKS_API UOdysseyAnimationTimelineSection
    : public UMovieSceneSection
{
    GENERATED_BODY()

public:
    static TRange<FFrameNumber> GetDefaultSectionRange(UOdysseyAnimationTimelineSection* iSection);

public:
    UOdysseyAnimationTimelineSection(const FObjectInitializer& ObjInitializer);

protected:
    virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
    virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override;
public:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    FFrameNumber StartFrameOffset = 0;
};
