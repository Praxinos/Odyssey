// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Evaluation/MovieSceneEvalTemplate.h"
#include "OdysseyAnimationComponentSection.h"
#include "OdysseyAnimationComponentTrack.h"

#include "OdysseyAnimationComponentTemplate.generated.h"

USTRUCT()
struct FOdysseyAnimationComponentSectionParams
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

	FOdysseyAnimationComponentSectionParams()
		: bLooping(false)
	{}
};


USTRUCT()
struct FOdysseyAnimationComponentTemplate
	: public FMovieSceneEvalTemplate
{
	GENERATED_BODY()

	/** Default constructor. */
	FOdysseyAnimationComponentTemplate() { }

	/**
	 * Create and initialize a new instance.
	 *
	 * @param InSection
	 * @param InTrack
	 */
	FOdysseyAnimationComponentTemplate(const UOdysseyAnimationComponentSection& InSection, const UOdysseyAnimationComponentTrack& InTrack);

public:

	//~ FMovieSceneEvalTemplate interface

	virtual void Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const override;
	virtual UScriptStruct& GetScriptStructImpl() const override;
	virtual void Initialize(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;
	virtual void SetupOverrides() override;
	virtual void TearDown(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const override;

private:
	UPROPERTY()
	FOdysseyAnimationComponentSectionParams mParams;

	UPROPERTY()
	TObjectPtr<const UOdysseyAnimationComponentSection> mSection;
};
