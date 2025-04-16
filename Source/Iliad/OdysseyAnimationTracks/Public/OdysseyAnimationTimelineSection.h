// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "MovieSceneSection.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"

#include "OdysseyAnimationCut.h"
#include "OdysseyAnimationCutChannel.h"

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

public:
    UOdysseyAnimation* GetAnimation() const;
    EOdysseyAnimationPlayerPostBehaviour GetPreBehaviour() const;
    EOdysseyAnimationPlayerPostBehaviour GetPostBehaviour() const;
    FFrameNumber GetStartFrameOffset() const;

    void SetAnimation(UOdysseyAnimation* iAnimation);
    void SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);
    void SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue);
    void SetStartFrameOffset(FFrameNumber iOffset);

    FFrameNumber ConvertFrameFromTimelineToSequence( FFrameNumber iFrameInTimeline );
    FFrameNumber ConvertFrameFromSequenceToTimeline( FFrameNumber iFrameInSequence );

    FOdysseyAnimationCutChannel& GetAnimationCutChannel();
    const FOdysseyAnimationCutChannel& GetAnimationCutChannel() const;

    virtual void RebuildAnimationCuts();

protected:
    virtual EMovieSceneChannelProxyType CacheChannelProxy() override;
    virtual void MigrateFrameTimes(FFrameRate SourceRate, FFrameRate DestinationRate) override;

    virtual void BuildAnimationCuts();

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    FFrameNumber StartFrameOffset = 0;

    UPROPERTY()
    FOdysseyAnimationCutChannel AnimationCutChannel;
};
