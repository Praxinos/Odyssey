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
    FOdysseyAnimationTimelineSectionExecutionToken(const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate)
        : mRange(iRange)
        , mParams(iParams)
        , mFrameRate(iFrameRate)
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

        Execute(component, mRange, mParams, mFrameRate);
    }

#if PLATFORM_WINDOWS
    __declspec(noinline)
#endif
    static void Execute(UOdysseyAnimationComponent* iComponent, const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate)
    {
        if (!iComponent)
            return;

        UOdysseyAnimation* animation = iParams.Animation;
        if (!animation)
            return;

        UOdysseyAnimationPlayer* player = iComponent->GetActivePlayer();
        if (!player)
            return;

        iComponent->SetMode(EOdysseyAnimationComponentMode::Animation);
        iComponent->SetAnimation(iParams.Animation);
        player->SetPreBehaviour(iParams.PreBehaviour);
        player->SetPostBehaviour(iParams.PostBehaviour);

        FFrameRate animationFrameRate(animation->GetFramesPerSecond() * 100, 100);

        FFrameTime rangeStartFrame = iRange.GetLowerBoundValue() - iParams.SectionStartFrame + iParams.StartFrameOffset;
        FFrameTime rangeEndFrame = iRange.GetUpperBoundValue() - iParams.SectionStartFrame + iParams.StartFrameOffset;

        FFrameTime startFrame = FFrameRate::TransformTime(rangeStartFrame, iFrameRate, animationFrameRate);
        FFrameTime endFrame = FFrameRate::TransformTime(rangeEndFrame, iFrameRate, animationFrameRate);

        FFrameNumber frame = startFrame.GetFrame();
        if (startFrame.GetFrame() == endFrame.GetFrame())
        {
            //Do Nothing
        }
        else if (endFrame.GetFrame() == startFrame.GetFrame() + 1)
        {
            frame = endFrame.GetSubFrame() > startFrame.GetSubFrame() ? endFrame.GetFrame() : startFrame.GetFrame();
        }
        else if (endFrame.GetFrame() > startFrame.GetFrame() + 1)
        {
            frame = startFrame.GetSubFrame() > 1.f - UE_SMALL_NUMBER ? startFrame.GetFrame() : startFrame.GetFrame() + 1;
        }

        player->SeekToFrameImmediate(frame.Value);
        if (frame != animation->CurrentFrame)
            FObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frame.Value);
    }

private:
    TRange<FFrameTime> mRange;
    FOdysseyAnimationTimelineSectionParams mParams;
    FFrameRate mFrameRate;
};


/* FOdysseyAnimationTimelineTemplate structors
 *****************************************************************************/

FOdysseyAnimationTimelineTemplate::FOdysseyAnimationTimelineTemplate(const UOdysseyAnimationTimelineSection& InSection, const UOdysseyAnimationTimelineTrack& InTrack)
    : mSection(&InSection)
{
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

    if (Context.GetTime().FrameNumber > mSection->GetRange().GetUpperBoundValue() || Context.GetTime().FrameNumber < mSection->GetRange().GetLowerBoundValue())
        return;

    FOdysseyAnimationTimelineSectionParams params;
    params.SectionStartFrame = mSection->GetRange().GetLowerBoundValue();
    params.SectionEndFrame = mSection->GetRange().GetUpperBoundValue();
    params.StartFrameOffset = mSection->GetStartFrameOffset();
    params.Animation = mSection->GetAnimation();
    params.PreBehaviour = mSection->GetPreBehaviour();
    params.PostBehaviour = mSection->GetPostBehaviour();

    ExecutionTokens.Add(FOdysseyAnimationTimelineSectionExecutionToken(Context.GetRange(), params, Context.GetFrameRate()));
}

void
FOdysseyAnimationTimelineTemplate::EvaluateImmediate(UOdysseyAnimationComponent* iComponent, const TRange<FFrameTime>& iRange, const FOdysseyAnimationTimelineSectionParams& iParams, const FFrameRate& iFrameRate )
{
    FOdysseyAnimationTimelineSectionExecutionToken::Execute(iComponent, iRange, iParams, iFrameRate);
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
