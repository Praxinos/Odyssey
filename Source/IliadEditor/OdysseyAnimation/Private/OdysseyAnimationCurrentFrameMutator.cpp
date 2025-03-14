// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationCurrentFrameMutator.h"

#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/OdysseyUndoDelegates.h"

FOdysseySetCurrentFrameMutation::FOdysseySetCurrentFrameMutation(UOdysseyAnimation* iAnimation, int iFrame)
    : mAnimation(iAnimation)
    , mFrame(iFrame)
{
}

void
FOdysseySetCurrentFrameMutation::Apply()
{
    mAnimation->CurrentFrame = mFrame;
    mAnimation->PropertyChanged(GET_MEMBER_NAME_CHECKED( UOdysseyAnimation, CurrentFrame));
    FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
        [animation = mAnimation](bool iIsRedo)
        {
            animation->PostPropertyChanged(GET_MEMBER_NAME_CHECKED( UOdysseyAnimation, CurrentFrame));
        }
    );
}

void
FOdysseySetCurrentFrameMutation::Revert()
{
    mAnimation->CurrentFrame = mFrame;
    mAnimation->PropertyChanged(GET_MEMBER_NAME_CHECKED( UOdysseyAnimation, CurrentFrame));
    FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
        [animation = mAnimation](bool iIsRedo)
        {
            animation->PostPropertyChanged(GET_MEMBER_NAME_CHECKED( UOdysseyAnimation, CurrentFrame));
        }
    );
}

FOdysseyAnimationCurrentFrameMutator::FOdysseyAnimationCurrentFrameMutator(UOdysseyAnimation* iAnimation)
    : FOdysseyMutator(iAnimation, "FOdysseyAnimationCurrentFrameMutator")
    , mAnimation(iAnimation)
{
}

void
FOdysseyAnimationCurrentFrameMutator::Set(int iFrame)
{
    TSharedRef<FOdysseySetCurrentFrameMutation> mutation = MakeShared<FOdysseySetCurrentFrameMutation>(mAnimation, iFrame);
    AddMutation(mutation);
    if (iFrame != mAnimation->CurrentFrame)
        ApplyMutation(mutation);
}
