// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "Sections/MovieSceneSubSection.h"
#include "OdysseyAnimationComponentSection.generated.h"

enum class EMovieSceneChannelProxyType : uint8;

class UOdysseyAnimationComponent;

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

public:
    virtual void PostInitProperties() override;

protected:
    virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
    virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    FFrameNumber StartFrameOffset = 0;
};
