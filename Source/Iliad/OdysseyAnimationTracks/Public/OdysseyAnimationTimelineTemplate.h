// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Evaluation/MovieSceneEvalTemplate.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineTrack.h"

#include "OdysseyAnimationTimelineTemplate.generated.h"

USTRUCT()
struct FOdysseyAnimationTimelineSectionParams
{
    GENERATED_BODY()

    UPROPERTY()
    FFrameNumber SectionStartFrame;

    UPROPERTY()
    FFrameNumber SectionEndFrame;

    UPROPERTY()
    FFrameNumber StartFrameOffset;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PreBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    UPROPERTY()
    EOdysseyAnimationPlayerPostBehaviour PostBehaviour = EOdysseyAnimationPlayerPostBehaviour::Loop;

    FOdysseyAnimationTimelineSectionParams()
    {}
};

//---

class UOdysseyAnimationComponent;

USTRUCT()
struct ODYSSEYANIMATIONTRACKS_API FOdysseyAnimationTimelineTemplate
    : public FMovieSceneEvalTemplate
{
    GENERATED_BODY()

    /** Default constructor. */
    FOdysseyAnimationTimelineTemplate() { }

    /**
     * Create and initialize a new instance.
     *
     * @param InSection
     * @param InTrack
     */
    FOdysseyAnimationTimelineTemplate(const UOdysseyAnimationTimelineSection& InSection, const UOdysseyAnimationTimelineTrack& InTrack);

public:
    //~ FMovieSceneEvalTemplate interface
    virtual void Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;
    virtual UScriptStruct& GetScriptStructImpl() const override;
    virtual void Initialize(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;
    virtual void SetupOverrides() override;
    virtual void TearDown(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;

public:
    static FFrameNumber GetEvaluatedFrame(UOdysseyAnimation* iComponent, const TRange<FFrameTime>& iRange, EPlayDirection iDirection, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate);
    static void EvaluateImmediate(UOdysseyAnimationComponent* iComponent, const TRange<FFrameTime>& iRange, EPlayDirection iDirection, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate );

private:
    UPROPERTY()
    TObjectPtr<const UOdysseyAnimationTimelineSection> mSection;
};
