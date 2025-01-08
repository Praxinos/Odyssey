// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
    bool bLooping;

    FOdysseyAnimationTimelineSectionParams()
        : bLooping(false)
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
    static void GetStartTimeAndDuration(const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate, double& oStartTime, double& oDuration);
    static void EvaluateImmediate(UOdysseyAnimationComponent* iComponent, const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate );

private:
    UPROPERTY()
    FOdysseyAnimationTimelineSectionParams mParams;

    UPROPERTY()
    TObjectPtr<const UOdysseyAnimationTimelineSection> mSection;
};
