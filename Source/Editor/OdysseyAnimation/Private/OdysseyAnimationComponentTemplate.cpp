// Copyright Epic Games, Inc. All Rights Reserved.

#include "OdysseyAnimationComponentTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationComponentTemplate)

#define MOVIESCENEMEDIATEMPLATE_TRACE_EVALUATION 0


/* Local helpers
 *****************************************************************************/

struct FOdysseyAnimationComponentSectionExecutionToken
	: IMovieSceneExecutionToken
{
	FOdysseyAnimationComponentSectionExecutionToken(double iStartTime, double iDuration)
		: mStartTime(iStartTime)
		, mDuration(iDuration)
	{ }

	virtual void Execute(const FMovieSceneContext& Context, const FMovieSceneEvaluationOperand& Operand, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) override
	{
		// Nope... do we have an object binding?
		UOdysseyAnimationComponent* component = nullptr;
		if (Operand.ObjectBindingID.IsValid())
		{
			// Yes. Get the media player from the object.
			for (TWeakObjectPtr<> WeakObject : Player.FindBoundObjects(Operand))
			{
				UObject* BoundObject = WeakObject.Get();
				if (!BoundObject || !BoundObject->IsA<UOdysseyAnimationComponent>())
					continue;

				component = Cast<UOdysseyAnimationComponent>(BoundObject);
					break;
			}
		}

		if (!component)
			return;

		UOdysseyAnimation* animation = component->GetActiveAnimation();
		if (!animation)
			return;

		UOdysseyAnimationPlayer* player = component->GetActivePlayer();
		if (!player)
			return;

		double startFrame = animation->GetFramesPerSecond() * mStartTime;
		double endFrame = animation->GetFramesPerSecond() * (mStartTime + mDuration);

		int startFrameInt = FMath::Floor(startFrame);
		int endFrameInt = FMath::Floor(endFrame);

		int frame = startFrameInt;
		if (endFrameInt == startFrameInt + 1)
		{
			double startOverlap = 1.0f - (startFrame - startFrameInt);
			double endOverlap = endFrame - endFrameInt;

			frame = startOverlap > endOverlap ? startFrameInt : endFrameInt;
		}
		else if (endFrameInt > startFrameInt + 1)
		{
			double startOverlap = 1.0f - (startFrame - startFrameInt);
			frame = startOverlap > 1.f - UE_SMALL_NUMBER ? startFrameInt : startFrameInt + 1;
		}

		player->SeekToFrame(frame);
		if (frame != animation->CurrentFrame)
			FObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frame);
	}

private:
	double mStartTime;
	double mDuration;
};


/* FOdysseyAnimationComponentTemplate structors
 *****************************************************************************/

FOdysseyAnimationComponentTemplate::FOdysseyAnimationComponentTemplate(const UOdysseyAnimationComponentSection& InSection, const UOdysseyAnimationComponentTrack& InTrack)
	: mSection(&InSection)
{
	if (InSection.HasStartFrame())
	{
		mParams.SectionStartFrame = InSection.GetRange().GetLowerBoundValue();
	}
	if (InSection.HasEndFrame())
	{
		mParams.SectionEndFrame = InSection.GetRange().GetUpperBoundValue();
	}
	mParams.bLooping = 0.f; //InSection.bLooping;
	mParams.StartFrameOffset = InSection.StartFrameOffset;
}


/* FMovieSceneEvalTemplate interface
 *****************************************************************************/

void
FOdysseyAnimationComponentTemplate::Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
	if (Context.IsPreRoll())
		return;

	if (Context.IsPostRoll())
		return;
	
	if (Context.GetTime().FrameNumber > mParams.SectionEndFrame || Context.GetTime().FrameNumber < mParams.SectionStartFrame)
		return;
		
	const FFrameRate FrameRate = Context.GetFrameRate();
	const FFrameTime FrameTime(Context.GetTime().FrameNumber - mParams.SectionStartFrame + mParams.StartFrameOffset);
	const double FrameTimeInSeconds = FrameRate.AsSeconds(FrameTime);

	// With zero-length frames (which can occur occasionally), we use the fixed frame time, matching previous behavior.
	const double FrameDurationInSeconds = FMath::Clamp( (Context.GetRange().Size<FFrameTime>()) / Context.GetFrameRate(), FrameRate.AsSeconds(FFrameTime(1)), FrameRate.AsSeconds((mParams.SectionEndFrame - mParams.SectionStartFrame) - FrameTime));
	
	ExecutionTokens.Add(FOdysseyAnimationComponentSectionExecutionToken(FrameTimeInSeconds, FrameDurationInSeconds));
}


UScriptStruct&
FOdysseyAnimationComponentTemplate::GetScriptStructImpl() const
{
	return *StaticStruct();
}


void
FOdysseyAnimationComponentTemplate::Initialize(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const
{
}


void
FOdysseyAnimationComponentTemplate::SetupOverrides()
{
	EnableOverrides(RequiresInitializeFlag | RequiresTearDownFlag);
}


void
FOdysseyAnimationComponentTemplate::TearDown(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const
{
}

