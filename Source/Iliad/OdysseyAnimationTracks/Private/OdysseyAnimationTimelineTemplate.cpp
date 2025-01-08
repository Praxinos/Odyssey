// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationTimelineTemplate.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OdysseyAnimationTimelineTemplate)

#define MOVIESCENEMEDIATEMPLATE_TRACE_EVALUATION 0

/* Local helpers
 *****************************************************************************/

struct FOdysseyAnimationTimelineSectionExecutionToken
    : IMovieSceneExecutionToken
{
    FOdysseyAnimationTimelineSectionExecutionToken(double iStartTime, double iDuration)
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

        Execute(component, mStartTime, mDuration);
    }

    __declspec(noinline) static void Execute(UOdysseyAnimationComponent* iComponent, double iStartTime, double iDuration)
    {
        if (!iComponent)
            return;

        UOdysseyAnimation* animation = iComponent->GetActiveAnimation();
        if (!animation)
            return;

        UOdysseyAnimationPlayer* player = iComponent->GetActivePlayer();
        if (!player)
            return;

        double startFrame = animation->GetFramesPerSecond() * iStartTime;
        double endFrame = animation->GetFramesPerSecond() * (iStartTime + iDuration);

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


/* FOdysseyAnimationTimelineTemplate structors
 *****************************************************************************/

FOdysseyAnimationTimelineTemplate::FOdysseyAnimationTimelineTemplate(const UOdysseyAnimationTimelineSection& InSection, const UOdysseyAnimationTimelineTrack& InTrack)
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
FOdysseyAnimationTimelineTemplate::Evaluate(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, const FPersistentEvaluationData& PersistentData, FMovieSceneExecutionTokens& ExecutionTokens) const
{
    if (Context.IsPreRoll())
        return;

    if (Context.IsPostRoll())
        return;

    if (Context.GetTime().FrameNumber > mParams.SectionEndFrame || Context.GetTime().FrameNumber < mParams.SectionStartFrame)
        return;

    double startTime = 0.f;
    double duration = 0.f;
    GetStartTimeAndDuration(Context.GetRange(), mParams, Context.GetFrameRate(), startTime, duration);
    ExecutionTokens.Add(FOdysseyAnimationTimelineSectionExecutionToken(startTime, duration));
}

void
FOdysseyAnimationTimelineTemplate::GetStartTimeAndDuration(const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate, double& oStartTime, double& oDuration)
{
    const FFrameTime FrameTime(iRange.GetLowerBoundValue() - iParams.SectionStartFrame + iParams.StartFrameOffset);
    oStartTime = iFrameRate.AsSeconds(FrameTime);

    // With zero-length frames (which can occur occasionally), we use the fixed frame time, matching previous behavior.
    oDuration = FMath::Clamp( (iRange.Size<FFrameTime>()) / iFrameRate, iFrameRate.AsSeconds(FFrameTime(1)), iFrameRate.AsSeconds((iParams.SectionEndFrame - iParams.SectionStartFrame) - FrameTime));
}

void
FOdysseyAnimationTimelineTemplate::EvaluateImmediate(UOdysseyAnimationComponent* iComponent, const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate )
{
    double startTime = 0.f;
    double duration = 0.f;
    GetStartTimeAndDuration(iRange, iParams, iFrameRate, startTime, duration);
    FOdysseyAnimationTimelineSectionExecutionToken::Execute(iComponent, startTime, duration);
}

UScriptStruct&
FOdysseyAnimationTimelineTemplate::GetScriptStructImpl() const
{
    return *StaticStruct();
}

void
FOdysseyAnimationTimelineTemplate::Initialize(const FMovieSceneEvaluationOperand& Operand, const FMovieSceneContext& Context, FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const
{
}

void
FOdysseyAnimationTimelineTemplate::SetupOverrides()
{
    EnableOverrides(RequiresInitializeFlag | RequiresTearDownFlag);
}

void
FOdysseyAnimationTimelineTemplate::TearDown(FPersistentEvaluationData& PersistentData, IMovieScenePlayer& Player) const
{
}
